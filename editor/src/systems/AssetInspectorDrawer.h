#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/Material.h"
#include "origo/assets/Metadata.h"

namespace OrigoEditor {

class AssetInspectorDrawer {
public:
	static void Draw(const Origo::AssetMetadata& md);

private:
	static void DrawSpecific(Origo::Asset* asset, Origo::AssetType type);
	static void DrawMaterial(Origo::Material2D* material);
};

}
