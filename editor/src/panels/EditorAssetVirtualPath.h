#pragma once

#include "origo/assets/Metadata.h"

namespace OrigoEditor {

std::filesystem::path compute_virtual_asset_path(const Origo::AssetMetadata& metadata);
std::string asset_type_to_string(Origo::AssetType type);

}
