#include "origo/assets/AssetDatabase.h"
#include "magic_enum/magic_enum.hpp"
#include "origo/core/Logger.h"
#include "origo/serialization/ISerializer.h"
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

	serializer.WriteFile();
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

std::filesystem::path AssetDatabase::GetMetadataPath(const AssetMetadata& meta) {
	if (!meta.SourcePath.empty()) {
		return meta.SourcePath.string() + ".meta";
	}

	return ROOT / "generated" / (meta.Id.ToString() + ".meta");
}

}
