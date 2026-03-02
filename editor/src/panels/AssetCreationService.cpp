#include "panels/AssetCreationService.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/Material2D.h"
#include "origo/assets/Material2DSource.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/serialization/JsonSerializer.h"

namespace OrigoEditor {

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

bool AssetCreationService::ensure_physical_folder(const FolderEntry* current_folder, const char* error_message) {
	if (current_folder && std::filesystem::exists(current_folder->Path))
		return true;

	ORG_ERROR(error_message);
	return false;
}

bool AssetCreationService::create_material(FolderEntry* current_folder) const {
	using namespace Origo;

	if (!ensure_physical_folder(current_folder, "Unable to create materials within a virtual path"))
		return false;

	const auto unique_path = make_unique_asset_path(current_folder->Path, "material", ".mat");

	const auto material_handle = AssetFactory::get_instance().create_pathed_runtime_asset<Material2D>(unique_path.stem().string(), unique_path);
	auto material = AssetManager::get_instance().get_asset<Material2D>(material_handle);

	auto material_source = MakeScope<Material2DSourceRaw>(
	    DefaultAssetCache::get_instance().get_texture(),
	    DefaultAssetCache::get_instance().get_shader());

	material->set_source(std::move(material_source));
	material->make_textured_material();
	material->resolve();

	auto serializer { Origo::AssetSerializationSystem::get(AssetType::Material2D) };

	JsonSerializer backend { unique_path };
	serializer->serialize(material, backend);
	backend.save_to_file();

	return true;
}

bool AssetCreationService::create_script(FolderEntry* current_folder) const {
	if (!ensure_physical_folder(current_folder, "Unable to create shaders within a virtual path"))
		return false;

	return false;
}

bool AssetCreationService::create_shader(FolderEntry* current_folder) const {
	if (!ensure_physical_folder(current_folder, "Unable to create shaders within a virtual path"))
		return false;

	return false;
}

}
