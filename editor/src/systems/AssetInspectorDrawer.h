#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/Material2D.h"
#include "origo/assets/Metadata.h"

namespace OrigoEditor {

class AssetInspectorDrawer {
public:
	static void draw_asset(const Origo::AssetMetadata& md);

private:
	static void draw_specific(Origo::Asset* asset, Origo::AssetType type);
	static void draw_material(Origo::Material2D* material);
};

}
