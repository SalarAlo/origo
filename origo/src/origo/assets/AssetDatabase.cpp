#include "origo/assets/AssetDatabase.h"
#include "magic_enum/magic_enum.hpp"
#include "origo/serialization/JsonSerializer.h"
#include "origo/assets/AssetMetadata.h"

namespace Origo {

void AssetDatabase::WriteMetadata(const AssetMetadata& meta) {
	s_Metadata[meta.Id] = meta;
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

std::filesystem::path AssetDatabase::GetMetadataPath(const AssetMetadata& meta) {
	if (!meta.SourcePath.empty()) {
		return meta.SourcePath.string() + ".meta";
	}

	return s_Root / "generated" / (meta.Id.ToString() + ".meta");
}

}
