#include "origo/assets/AssetDatabase.h"
#include "magic_enum/magic_enum.hpp"
#include "origo/serialization/JsonSerializer.h"
#include "origo/assets/AssetMetadata.h"
#include <cctype>

static void ToLowerInPlace(std::string& s) {
	std::ranges::transform(
	    s, s.begin(),
	    [](unsigned char c) { return std::tolower(c); });
}

namespace Origo {

void AssetDatabase::WriteMetadata(const AssetMetadata& meta) {
	s_Metadata[meta.Id] = meta;
	auto path { GetMetadataPath(meta.Id) };

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

std::filesystem::path AssetDatabase::GetMetadataPath(const UUID& id) {
	auto it { s_Metadata.find(id) };
	const auto& meta = it->second;

	std::string originTypeStr { magic_enum::enum_name(meta.Origin) };
	ToLowerInPlace(originTypeStr);
	std::string assetTypeStr { magic_enum::enum_name(meta.Type) };
	ToLowerInPlace(assetTypeStr);

	return s_Root / assetTypeStr / originTypeStr / (meta.Name + "_" + meta.Id.ToString() + ".meta");
}

}
