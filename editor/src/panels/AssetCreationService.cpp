#include "panels/AssetCreationService.h"

#include <cctype>
#include <fstream>

#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/Prefab.h"
#include "origo/assets/Script.h"

#include "origo/assets/material/MaterialPBR.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/core/PathContext.h"

#include "origo/serialization/JsonSerializer.h"

#include "ui/EditorNotificationSystem.h"

namespace OrigoEditor {

namespace {

std::string make_component_name_from_stem(std::string stem) {
	if (stem.empty())
		return "ScriptComponent";

	std::string result;
	result.reserve(stem.size() + 9);
	bool capitalize_next = true;

	for (char ch : stem) {
		if (std::isalnum(static_cast<unsigned char>(ch))) {
			result.push_back(capitalize_next ? static_cast<char>(std::toupper(static_cast<unsigned char>(ch))) : ch);
			capitalize_next = false;
			continue;
		}

		capitalize_next = true;
	}

	if (result.empty())
		result = "Script";

	result += "Component";
	return result;
}

std::string make_default_noct_script(const std::filesystem::path& path) {
	const std::string component_name = make_component_name_from_stem(path.stem().string());
	return "import Component\n\n"
	       "Component.define(\n"
	       "\t\"" + component_name + "\",\n"
	       "\t\"enabled\", \"Bool\", true\n"
	       ");\n";
}

}

std::filesystem::path AssetCreationService::make_unique_asset_path(const std::filesystem::path& folder, const std::string& base_name, const std::string& extension) {
	auto& db = Origo::AssetDatabase::get_instance().get_all_metadata();

	auto exists_in_metadata = [&](const std::filesystem::path& path) {
		for (const auto& [id, meta] : db) {
			if (meta.SourcePath && meta.SourcePath->lexically_normal() == path.lexically_normal())
				return true;
		}
		return false;
	};

	std::filesystem::path candidate = folder / (base_name + extension);
	if (!std::filesystem::exists(candidate) && !exists_in_metadata(candidate))
		return candidate;

	int index = 1;
	while (true) {
		std::filesystem::path attempt = folder / (base_name + "_" + std::to_string(index) + extension);
		if (!std::filesystem::exists(attempt) && !exists_in_metadata(attempt))
			return attempt;
		++index;
	}
}

std::filesystem::path AssetCreationService::resolve_physical_folder(const FolderEntry* current_folder) {
	if (!current_folder)
		return {};

	const auto resolved = Origo::PathContextService::get_instance().resolve_serialized_path(current_folder->Path);
	if (resolved.empty() || !resolved.is_absolute())
		return {};

	return resolved;
}

bool AssetCreationService::ensure_physical_folder(
    const FolderEntry* current_folder,
    std::filesystem::path& resolved_folder,
    const char* error_message) {
	resolved_folder = resolve_physical_folder(current_folder);
	if (!resolved_folder.empty() && std::filesystem::exists(resolved_folder))
		return true;

	ORG_ERROR(error_message);
	EditorNotificationSystem::get_instance().warning(
	    "Create Asset Failed",
	    error_message);
	return false;
}

bool AssetCreationService::create_material(FolderEntry* current_folder) const {
	using namespace Origo;

	std::filesystem::path folder_path;
	if (!ensure_physical_folder(current_folder, folder_path, "Unable to create materials within a virtual path"))
		return false;

	const auto unique_path = make_unique_asset_path(folder_path, "material", ".matpbr");

	const auto material_handle = AssetFactory::get_instance().create_pathed_runtime_asset<MaterialPBR>(unique_path.stem().string(), unique_path);
	auto material = AssetManager::get_instance().get_asset<MaterialPBR>(material_handle);

	material->set_shader(DefaultAssetCache::get_instance().get_shader());
	material->set_albedo(DefaultAssetCache::get_instance().get_texture());

	material->resolve();

	auto serializer { Origo::AssetSerializationSystem::get(AssetType::MaterialPBR) };

	JsonSerializer backend { unique_path };
	serializer->serialize(material, backend);
	backend.save_to_file();
	EditorNotificationSystem::get_instance().success(
	    "Material Created",
	    "Created " + unique_path.filename().string());

	return true;
}

bool AssetCreationService::create_prefab(FolderEntry* current_folder, const Origo::RID& e, Origo::Scene* scene) const {
	using namespace Origo;

	std::filesystem::path folder_path;
	if (!ensure_physical_folder(current_folder, folder_path, "Unable to create prefabs within a virtual path"))
		return false;

	const auto unique_path = make_unique_asset_path(folder_path, "prefab", ".prefab");

	const auto prefab_handle = AssetFactory::get_instance().create_pathed_runtime_asset<Prefab>(unique_path.stem().string(), unique_path, e, scene);
	auto prefab = AssetManager::get_instance().get_asset<Prefab>(prefab_handle);

	auto serializer { Origo::AssetSerializationSystem::get(AssetType::Prefab) };

	JsonSerializer backend { unique_path };
	serializer->serialize(prefab, backend);

	backend.save_to_file();

	EditorNotificationSystem::get_instance().success(
	    "Prefab Created",
	    "Created " + unique_path.filename().string());

	return true;
}

bool AssetCreationService::create_script(FolderEntry* current_folder) const {
	using namespace Origo;

	std::filesystem::path folder_path;
	if (!ensure_physical_folder(current_folder, folder_path, "Unable to create scripts within a virtual path"))
		return false;

	const auto unique_path = make_unique_asset_path(folder_path, "script", ".noct");

	{
		std::ofstream out(unique_path, std::ios::binary);
		if (!out.is_open()) {
			EditorNotificationSystem::get_instance().warning(
			    "Script Creation Failed",
			    "Unable to write " + unique_path.filename().string());
			return false;
		}

		out << make_default_noct_script(unique_path);
		out.flush();
	}

	AssetMetadata meta {};
	meta.ID = UUID::generate();
	meta.Name = unique_path.stem().string();
	meta.Type = AssetType::Script;
	meta.Origin = AssetOrigin::Imported;
	meta.SourcePath = unique_path;
	meta.SourceTimestamp = std::filesystem::last_write_time(unique_path);
	meta.ImportedTimestamp = meta.SourceTimestamp;

	auto script = make_scope<Script>(unique_path, *meta.ID);
	auto handle = AssetFactory::get_instance().create_imported_asset(meta, std::move(script));
	auto* loaded_script = AssetManager::get_instance().get_asset<Script>(handle);
	if (loaded_script)
		loaded_script->resolve();

	AssetDatabase::get_instance().write_import_file(*meta.ID);

	EditorNotificationSystem::get_instance().success(
	    "Script Created",
	    "Created " + unique_path.filename().string());

	return true;
}

bool AssetCreationService::create_shader(FolderEntry* current_folder) const {
	std::filesystem::path folder_path;
	if (!ensure_physical_folder(current_folder, folder_path, "Unable to create shaders within a virtual path"))
		return false;

	EditorNotificationSystem::get_instance().warning(
	    "Shader Creation Unavailable",
	    "Shader asset creation is not implemented yet.");
	return false;
}

}
