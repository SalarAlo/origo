#include "AssetThumbnailGenerator.h"

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

#include "origo/core/Logger.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {
static ImTextureID GetTextureID(const AssetEntry* entry) {
	auto& am { Origo::AssetManager::GetInstance() };
	auto handle { am.GetHandleByUUID(entry->id) };

	if (!handle.has_value()) {
		ORG_ERROR("AssetThumbnailGenerator::GetThumbnailID: AssetEntry of type Texture2D is not found in asset manager!");
		return 0;
	}

	auto tex { am.Get<Origo::Texture2D>(*handle) };
	return static_cast<ImTextureID>((intptr_t)tex->GetRendererID());
}

ImTextureID AssetThumbnailGenerator::GetThumbnailID(const AssetEntry* entry) {
	switch (entry->type) {
	case Origo::AssetType::Texture2D:
		return GetTextureID(entry);
	default:
		return EditorIcons::GetInstance().Get(entry->type);
	}
}

}
