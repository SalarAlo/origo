#pragma once

#include <filesystem>
#include <string>

#include "panels/EditorAssetTree.h"

namespace OrigoEditor {

class AssetCreationService {
public:
	bool create_material(FolderEntry* current_folder) const;
	bool create_script(FolderEntry* current_folder) const;
	bool create_shader(FolderEntry* current_folder) const;

private:
	static std::filesystem::path make_unique_asset_path(const std::filesystem::path& folder, const std::string& base_name, const std::string& extension);
	static bool ensure_physical_folder(const FolderEntry* current_folder, const char* error_message);
};

}
