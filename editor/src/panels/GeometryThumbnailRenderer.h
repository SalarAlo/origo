#pragma once

#include <cstdint>
#include <unordered_map>

#include "origo/assets/AssetManager.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/RenderContext.h"

namespace OrigoEditor {

class GeometryThumbnailRenderer {
public:
	static GeometryThumbnailRenderer& get_instance();

	ImTextureID get_mesh_thumbnail(Origo::AssetHandle mesh_handle);
	ImTextureID get_model_thumbnail(Origo::AssetHandle model_handle);
	void invalidate(const Origo::UUID& asset_id);
	void invalidate_all();

private:
	enum class PreviewType {
		Mesh,
		Model
	};

	GeometryThumbnailRenderer();
	static std::uint64_t make_cache_key(Origo::AssetHandle asset_handle, PreviewType preview_type);

	GeometryThumbnailRenderer(const GeometryThumbnailRenderer&) = delete;
	GeometryThumbnailRenderer& operator=(const GeometryThumbnailRenderer&) = delete;

	ImTextureID get_thumbnail(Origo::AssetHandle asset_handle, PreviewType preview_type);
	void render_thumbnail(Origo::AssetHandle asset_handle, PreviewType preview_type);

private:
	struct ThumbnailCacheEntry {
		Origo::Scope<Origo::FrameBuffer> Buffer {};
		bool Dirty { true };
	};

	Origo::RenderContext m_render_context;
	std::unordered_map<std::uint64_t, ThumbnailCacheEntry> m_thumbnail_cache {};
};

}
