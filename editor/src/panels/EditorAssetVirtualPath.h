#pragma once

#include "origo/assets/Metadata.h"

namespace OrigoEditor {

std::filesystem::path ComputeVirtualAssetPath(const Origo::AssetMetadata& metadata);
std::string AssetTypeToString(Origo::AssetType type);

}
