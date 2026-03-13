
#pragma once

#include "origo/assets/Metadata.h"

#include "origo/utils/Singleton.h"

namespace Origo {

class Asset;

class AssetDatabase : public Singleton<AssetDatabase> {
public:
	const std::unordered_map<UUID, AssetMetadata>& get_all_metadata() { return m_metadata; }
	void write_import_file(const UUID& id);
	const AssetMetadata& get_metadata(const UUID& id);
	void register_metadata(const AssetMetadata& meta);
	AssetMetadata load_import_header(const std::filesystem::path& path);
	Asset* load_asset(const UUID& id);
	void save_assets();

private:
	std::filesystem::path get_import_path(const AssetMetadata& meta);

private:
	std::unordered_map<UUID, AssetMetadata> m_metadata {};
};

}
