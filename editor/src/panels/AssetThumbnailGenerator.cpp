#include "AssetThumbnailGenerator.h"

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Texture2D.h"

#include "origo/core/Logger.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {

static ImTextureID to_im_texture_id(Origo::Texture2D* texture) {
	if (!texture)
		return 0;

	return static_cast<ImTextureID>((intptr_t)texture->get_renderer_id());
}

static ImTextureID get_texture_id(const AssetEntry* entry) {
	auto& am { Origo::AssetManager::get_instance() };
	auto handle { am.get_handle_by_uuid(entry->Id) };

	if (!handle.has_value()) {
		ORG_ERROR("AssetThumbnailGenerator::GetThumbnailID: AssetEntry of type Texture2D is not found in asset manager!");
		return 0;
	}

	auto tex { am.get_asset<Origo::Texture2D>(*handle) };
	return to_im_texture_id(tex);
}

ImTextureID AssetThumbnailGenerator::get_thumbnail_id(const AssetEntry* entry) {
	switch (entry->Type) {
	case Origo::AssetType::Texture2D:
		return get_texture_id(entry);
	default:
		return EditorIcons::get_instance().get(entry->Type);
	}
}

}
