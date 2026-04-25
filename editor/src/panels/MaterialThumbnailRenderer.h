#pragma once

#include <cstdint>
#include <unordered_map>

#include "origo/assets/AssetManager.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/RenderContext.h"

namespace OrigoEditor {

class MaterialThumbnailRenderer {
public:
	static MaterialThumbnailRenderer& get_instance();

	ImTextureID get_thumbnail(Origo::AssetHandle material_handle);
	void invalidate(const Origo::UUID& material_id);
	void invalidate_all();

private:
	MaterialThumbnailRenderer();
	static std::uint64_t make_cache_key(Origo::AssetHandle material_handle);

	MaterialThumbnailRenderer(const MaterialThumbnailRenderer&) = delete;
	MaterialThumbnailRenderer& operator=(const MaterialThumbnailRenderer&) = delete;

	void render_thumbnail(Origo::AssetHandle material_handle);

private:
	struct ThumbnailCacheEntry {
		Origo::Scope<Origo::FrameBuffer> Buffer {};
		bool Dirty { true };
	};

	static constexpr int k_thumbnail_size = 128;

	Origo::RenderContext m_render_context;
	std::unordered_map<std::uint64_t, ThumbnailCacheEntry> m_thumbnail_cache {};
};

}
