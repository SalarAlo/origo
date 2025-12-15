#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/core/Logger.h"
#include "origo/serialization/JsonSerializer.h"
#include "magic_enum/magic_enum.hpp"

namespace Origo {

void AssetDatabase::RegisterMetadata(const Metadata& meta) {
	s_Metadata[meta.Id] = meta;
}

void AssetDatabase::WriteImportFile(const UUID& id) {
	auto metaIt = s_Metadata.find(id);
	if (metaIt == s_Metadata.end()) {
		ORG_ERROR("AssetDatabase::WriteImportFile: No metadata for UUID {}", id.ToString());
		return;
	}

	const Metadata& meta = metaIt->second;

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

	serializer.BeginObject("Header");
	serializer.Write("UUID", meta.Id.ToString());
	serializer.Write("Name", meta.Name);
	serializer.Write("Type", magic_enum::enum_name(meta.Type));
	serializer.Write("Origin", magic_enum::enum_name(meta.Origin));
	serializer.Write("SourcePath", meta.SourcePath.string());
	serializer.EndObject();

	serializer.BeginObject("Payload");
	auto assetSerializer = AssetSerializationSystem::Get(meta.Type);
	assetSerializer->Serialize(asset, serializer);
	serializer.EndObject();

	serializer.SaveToFile();
}

Metadata AssetDatabase::LoadImportHeader(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		ORG_ERROR("Import file '{}' does not exist", path.string());
		return {};
	}

	JsonSerializer backend { path.string() };
	backend.LoadFile();

	Metadata meta;

	backend.BeginObject("Header");

	std::string uuidStr;
	backend.TryRead("UUID", uuidStr);
	meta.Id = UUID::FromString(uuidStr);

	backend.TryRead("Name", meta.Name);

	std::string typeStr;
	backend.TryRead("Type", typeStr);
	if (auto t = magic_enum::enum_cast<AssetType>(typeStr))
		meta.Type = *t;

	std::string originStr;
	backend.TryRead("Origin", originStr);
	if (auto o = magic_enum::enum_cast<AssetOrigin>(originStr))
		meta.Origin = *o;

	std::string source;
	backend.TryRead("SourcePath", source);
	meta.SourcePath = source;

	backend.BeginArray("Dependencies");
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

	const Metadata& meta = metaIt->second;
	auto path = GetImportPath(meta);

	JsonSerializer backend { path.string() };
	backend.LoadFile();

	backend.BeginObject("Header");
	backend.EndObject();

	backend.BeginObject("Payload");

	auto serializer = AssetSerializationSystem::Get(meta.Type);
	auto asset { AssetFactory::Create(meta.Type) };
	serializer->Deserialize(backend, *asset);

	backend.EndObject();

	auto handle = am.Register(std::move(asset), meta.Id);

	am.ResolveAll();

	return am.Get(handle);
}

void AssetDatabase::SaveAll() {
	auto& am = AssetManagerFast::GetInstance();

	for (const auto& [uuid, handle] : am.GetUuidMap()) {
		if (am.IsValid(handle))
			WriteImportFile(uuid);
	}
}

std::filesystem::path AssetDatabase::GetImportPath(const Metadata& meta) {
	if (!meta.SourcePath.empty()) {
		return meta.SourcePath.string() + ".import";
	}

	return ROOT / "generated" / (meta.Id.ToString() + ".import");
}

}
