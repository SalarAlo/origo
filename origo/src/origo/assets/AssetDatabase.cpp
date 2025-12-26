#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/core/Logger.h"
#include "origo/serialization/JsonSerializer.h"
#include "magic_enum/magic_enum.hpp"
#include <filesystem>

namespace Origo {

void AssetDatabase::RegisterMetadata(const AssetMetadata& meta) {
	s_Metadata[meta.ID] = meta;
}

void AssetDatabase::WriteImportFile(const UUID& id) {
	auto metaIt = s_Metadata.find(id);
	if (metaIt == s_Metadata.end()) {
		ORG_ERROR("AssetDatabase::WriteImportFile: No metadata for UUID {}", id.ToString());
		return;
	}

	const AssetMetadata& meta = metaIt->second;

	auto& am = AssetManagerFast::GetInstance();
	AssetHandle handle = am.Resolve(id);

	if (!am.IsValid(handle)) {
		ORG_WARN("Skipping save: asset {} not loaded", id.ToString());
		return;
	}

	Asset* asset = am.Get(handle);
	if (!asset) {
		ORG_WARN("Skipping save: asset {} invalid", id.ToString());
		return;
	}

	auto path = GetImportPath(meta);
	JsonSerializer serializer { path.string() };

	serializer.BeginObject("header");
	serializer.Write("uuid", meta.ID.ToString());
	serializer.Write("name", meta.Name);
	serializer.Write("type", magic_enum::enum_name(meta.Type));
	serializer.Write("origin", magic_enum::enum_name(meta.Origin));
	serializer.Write("source_path", meta.SourcePath.string());
	serializer.Write("source_time_stamp", std::to_string(meta.SourceTimestamp.time_since_epoch().count()));
	serializer.Write("imported_time_stamp", std::to_string(meta.ImportedTimestamp.time_since_epoch().count()));
	serializer.EndObject();

	serializer.BeginObject("payload");
	auto assetSerializer = AssetSerializationSystem::Get(meta.Type);
	assetSerializer->Serialize(asset, serializer);
	serializer.EndObject();

	serializer.SaveToFile();
}

AssetMetadata AssetDatabase::LoadImportHeader(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		ORG_ERROR("Import file '{}' does not exist", path.string());
		return {};
	}

	JsonSerializer backend { path.string() };
	backend.LoadFile();

	AssetMetadata meta;

	backend.BeginObject("header");

	std::string uuidStr;
	backend.TryRead("uuid", uuidStr);
	meta.ID = UUID::FromString(uuidStr);

	backend.TryRead("name", meta.Name);

	std::string typeStr;
	backend.TryRead("type", typeStr);
	if (auto t = magic_enum::enum_cast<AssetType>(typeStr))
		meta.Type = *t;

	std::string originStr;
	backend.TryRead("origin", originStr);
	if (auto o = magic_enum::enum_cast<AssetOrigin>(originStr))
		meta.Origin = *o;

	std::string source;
	if (backend.TryRead("source_path", source))
		meta.SourceTimestamp = std::filesystem::last_write_time(source);
	meta.SourcePath = source;

	std::string importedStamp;
	backend.TryRead("imported_time_stamp", importedStamp);
	meta.ImportedTimestamp = std::filesystem::file_time_type(std::filesystem::file_time_type::duration(std::stoll(importedStamp)));

	backend.BeginArray("dependencies");
	std::string dep;
	while (backend.TryReadArrayElement(dep)) {
		meta.Dependencies.push_back(UUID::FromString(dep));
	}
	backend.EndArray();

	backend.EndObject();

	return meta;
}

Asset* AssetDatabase::LoadAsset(const UUID& id) {
	auto& am { AssetManagerFast::GetInstance() };

	auto metaIt = s_Metadata.find(id);
	if (metaIt == s_Metadata.end()) {
		ORG_ERROR("AssetDatabase::LoadAsset: No metadata for uuid {}", id.ToString());
		return nullptr;
	}

	const AssetMetadata& meta = metaIt->second;
	auto path = GetImportPath(meta);

	JsonSerializer backend { path.string() };
	backend.LoadFile();

	backend.BeginObject("header");
	backend.EndObject();

	backend.BeginObject("payload");

	auto serializer = AssetSerializationSystem::Get(meta.Type);
	auto asset { AssetFactory::Create(meta.Type) };
	serializer->Deserialize(backend, *asset);

	backend.EndObject();

	auto handle = am.Register(std::move(asset), meta.ID);

	return am.Get(handle);
}

void AssetDatabase::SaveAll() {
	auto& am = AssetManagerFast::GetInstance();

	for (const auto& [uuid, handle] : am.GetUuidMap()) {
		if (am.IsValid(handle))
			WriteImportFile(uuid);
	}
}

std::filesystem::path AssetDatabase::GetImportPath(const AssetMetadata& meta) {
	if (!meta.SourcePath.empty()) {
		return meta.SourcePath.string() + ".import";
	}

	return ROOT / "generated" / (meta.ID.ToString() + ".import");
}

}
