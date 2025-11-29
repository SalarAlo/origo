#pragma once

#include "origo/core/RID.h"
#include "origo/assets/AssetMetadata.h"

namespace Origo {
class AssetDatabase {
public:
	AssetDatabase(const std::filesystem::path& root);
	static void Initialise();

private:
	void WriteMetadata(RID id, const AssetMetadata& meta);

private:
	inline static std::filesystem::path m_Root {};
	inline static std::unordered_map<RID, AssetMetadata> m_Metadata {};
};
}
