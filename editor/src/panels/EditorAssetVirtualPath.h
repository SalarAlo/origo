#pragma once

#include <unordered_map>

#include "origo/assets/Metadata.h"

namespace OrigoEditor {

std::filesystem::path compute_virtual_asset_path(const Origo::AssetMetadata& metadata);
std::filesystem::path compute_virtual_asset_path(
    const Origo::AssetMetadata& metadata,
    const std::unordered_map<Origo::UUID, const Origo::AssetMetadata*>& metadata_index);
std::string asset_type_to_string(Origo::AssetType type);

}
