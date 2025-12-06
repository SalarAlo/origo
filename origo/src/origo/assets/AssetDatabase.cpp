#include "origo/assets/AssetDatabase.h"
#include "magic_enum/magic_enum.hpp"
#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/core/Logger.h"
#include "origo/serialization/JsonSerializer.h"
#include "origo/assets/AssetMetadata.h"
#include <filesystem>

namespace Origo {

void AssetDatabase::WriteMetadata(const AssetMetadata& meta) {
	auto path { GetMetadataPath(meta) };

	JsonSerializer serializer { path.string() };

	serializer.Write("UUID", meta.Id.ToString());
	serializer.Write("Name", meta.Name);
	serializer.Write("Type", magic_enum::enum_name(meta.Type));
	serializer.Write("Origin", magic_enum::enum_name(meta.Origin));
	serializer.Write("SourcePath", meta.SourcePath.string());

	serializer.BeginArray("Dependencies");
	for (const auto& dependency : meta.Dependencies) {
		serializer.Write(dependency.ToString());
	}
	serializer.EndArray();

	serializer.SaveToFile();
}

void AssetDatabase::WriteAssetdata(const UUID& uuid) {
	auto asset = AssetManager::GetAsset(uuid);
	if (asset == nullptr) {
		ORG_INFO("AssetDatabase::WriteAssetdata: Non existent uuid {}", uuid.ToString());
		return;
	}

	auto serializer { AssetSerializationSystem::Get(asset->GetAssetType()) };
	auto path { GetAssetPath(s_Metadata[uuid]) };
	JsonSerializer backend { path.string() };
	serializer->Serialize(asset, backend);

	backend.SaveToFile();
}

void AssetDatabase::RegisterMetadata(const AssetMetadata& meta) {
	s_Metadata[meta.Id] = meta;
}

AssetMetadata AssetDatabase::LoadMetadata(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		ORG_INFO("Provided file {} doesnt exist", path.string());
		return {};
	}

	JsonSerializer backend { path.string() };
	backend.LoadFile();

	AssetMetadata meta;

	std::string uuidStr;
	backend.TryRead("UUID", uuidStr);
	meta.Id = UUID::FromString(uuidStr);

	backend.TryRead("Name", meta.Name);

	std::string typeStr;
	backend.TryRead("Type", typeStr);
	if (auto typeOpt = magic_enum::enum_cast<AssetType>(typeStr)) {
		meta.Type = *typeOpt;
	} else {
		ORG_ERROR("Invalid asset type '{}' in metadata '{}'", typeStr, path.string());
	}

	std::string originStr;
	backend.TryRead("Origin", originStr);
	if (auto originOpt = magic_enum::enum_cast<AssetOrigin>(originStr)) {
		meta.Origin = *originOpt;
	} else {
		ORG_ERROR("Invalid asset origin '{}' in metadata '{}'", originStr, path.string());
	}

	std::string sourcePath;
	backend.TryRead("SourcePath", sourcePath);
	meta.SourcePath = sourcePath;

	backend.BeginArray("Dependencies");
	std::string depStr;
	while (backend.TryRead("", depStr)) {
		meta.Dependencies.push_back(UUID::FromString(depStr));
	}
	backend.EndArray();

	return meta;
}

Asset* AssetDatabase::LoadAsset(const UUID& uuid) {
	auto it { s_Metadata.find(uuid) };
	if (it == s_Metadata.end()) {
		ORG_ERROR("AssetDatabase::LoadAsset: No metadata found with uuid {}", uuid.ToString());
		return nullptr;
	}

	const auto& meta { it->second };
	auto assetPath { GetAssetPath(meta) };

	JsonSerializer backend { assetPath.string() };
	backend.LoadFile();
	auto ser = AssetSerializationSystem::Get(meta.Type);
	Scope<Asset> asset { ser->Deserialize(backend) };

	auto id { AssetManager::Register(std::move(asset)) };

	return AssetManager::GetAsset(id);
}

std::filesystem::path AssetDatabase::GetMetadataPath(const AssetMetadata& meta) {
	if (!meta.SourcePath.empty()) {
		return meta.SourcePath.string() + ".meta";
	}

	return ROOT / "generated" / (meta.Id.ToString() + ".meta");
}
std::filesystem::path AssetDatabase::GetAssetPath(const AssetMetadata& meta) {
	if (!meta.SourcePath.empty()) {
		return meta.SourcePath.string() + ".asset";
	}

	return ROOT / "generated" / (meta.Id.ToString() + ".asset");
}

}
