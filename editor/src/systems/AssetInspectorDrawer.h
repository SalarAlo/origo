#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/material/MaterialPBR.h"
#include "origo/assets/Metadata.h"

#include "origo/utils/Singleton.h"

namespace OrigoEditor {

class AssetInspectorDrawer : public Origo::Singleton<AssetInspectorDrawer> {
public:
	void draw_asset(const Origo::AssetMetadata& md);

private:
	void draw_specific(Origo::Asset* asset, Origo::AssetType type);
	void draw_material(Origo::MaterialPBR* material);
};

}
