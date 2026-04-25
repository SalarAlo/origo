#include "AssetThumbnailGenerator.h"

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/material/MaterialPBR.h"
#include "origo/assets/model/Model.h"
#include "origo/assets/Texture2D.h"

#include "origo/core/Logger.h"

#include "panels/GeometryThumbnailRenderer.h"
#include "panels/MaterialThumbnailRenderer.h"

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

static ImTextureID get_material_thumbnail_id(const AssetEntry* entry) {
	auto& am { Origo::AssetManager::get_instance() };
	auto handle { am.get_handle_by_uuid(entry->Id) };

	if (!handle.has_value()) {
		ORG_ERROR("AssetThumbnailGenerator::GetThumbnailID: Material asset is not found in asset manager!");
		return EditorIcons::get_instance().get(entry->Type);
	}

	auto material { am.get_asset<Origo::MaterialPBR>(*handle) };
	if (!material)
		return EditorIcons::get_instance().get(entry->Type);

	material->resolve();
	return MaterialThumbnailRenderer::get_instance().get_thumbnail(*handle);
}

static ImTextureID get_geometry_thumbnail_id(const AssetEntry* entry) {
	auto& am { Origo::AssetManager::get_instance() };
	auto handle { am.get_handle_by_uuid(entry->Id) };

	if (!handle.has_value()) {
		ORG_ERROR("AssetThumbnailGenerator::GetThumbnailID: Geometry asset is not found in asset manager!");
		return EditorIcons::get_instance().get(entry->Type);
	}

	switch (entry->Type) {
	case Origo::AssetType::Mesh:
		return GeometryThumbnailRenderer::get_instance().get_mesh_thumbnail(*handle);
	case Origo::AssetType::Model: {
		auto* model = am.get_asset<Origo::Model>(*handle);
		if (!model || model->get_root_node() < 0 || model->get_nodes().empty() || model->get_sub_meshes().empty())
			return EditorIcons::get_instance().get(entry->Type);
		return GeometryThumbnailRenderer::get_instance().get_model_thumbnail(*handle);
	}
	default:
		return EditorIcons::get_instance().get(entry->Type);
	}
}

ImTextureID AssetThumbnailGenerator::get_thumbnail_id(const AssetEntry* entry) {
	switch (entry->Type) {
	case Origo::AssetType::Texture2D:
		return get_texture_id(entry);
	case Origo::AssetType::MaterialPBR:
		return get_material_thumbnail_id(entry);
	case Origo::AssetType::Mesh:
	case Origo::AssetType::Model:
		return get_geometry_thumbnail_id(entry);
	default:
		return EditorIcons::get_instance().get(entry->Type);
	}
}

}
