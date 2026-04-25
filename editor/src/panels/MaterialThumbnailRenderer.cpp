#include "panels/MaterialThumbnailRenderer.h"

#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/DirectionalLightData.h"
#include "origo/components/PointLightData.h"

#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/RenderView.h"

namespace {

Origo::FrameBufferSpec create_material_thumbnail_buffer_spec() {
	Origo::FrameBufferSpec spec;
	spec.Width = 128;
	spec.Height = 128;
	spec.Attachments = {
		{ Origo::AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
		{ Origo::AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
	};
	return spec;
}

Origo::RenderView create_material_thumbnail_view() {
	const Origo::Vec3 eye { 1.65f, 1.2f, 2.45f };
	const Origo::Vec3 target { 0.0f, 0.15f, 0.0f };
	const Origo::Vec3 up { 0.0f, 1.0f, 0.0f };

	Origo::RenderView view {};
	view.Projection = glm::perspective(glm::radians(32.0f), 1.0f, 0.1f, 100.0f);
	view.View = glm::lookAt(eye, target, up);
	view.CameraPosition = eye;
	view.CameraForward = glm::normalize(target - eye);
	return view;
}

Origo::DirectionalLightData create_thumbnail_directional_light() {
	Origo::DirectionalLightData light {};
	light.Direction = glm::normalize(Origo::Vec3(-0.55f, -1.0f, -0.35f));
	light.Color = { 1.0f, 0.97f, 0.92f };
	light.Intensity = 2.8f;
	light.Ambient = 0.24f;
	return light;
}

Origo::PointLightData create_thumbnail_rim_light() {
	Origo::PointLightData light {};
	light.Position = { -1.4f, 1.1f, 1.8f };
	light.Color = { 0.72f, 0.82f, 1.0f };
	light.Intensity = 18.0f;
	light.Linear = 0.12f;
	light.Quadratic = 0.05f;
	return light;
}

}

namespace OrigoEditor {

MaterialThumbnailRenderer& MaterialThumbnailRenderer::get_instance() {
	static MaterialThumbnailRenderer instance;
	return instance;
}

MaterialThumbnailRenderer::MaterialThumbnailRenderer()
    : m_render_context(nullptr) {
}

std::uint64_t MaterialThumbnailRenderer::make_cache_key(Origo::AssetHandle material_handle) {
	return (static_cast<std::uint64_t>(material_handle.Generation) << 32)
	    | static_cast<std::uint64_t>(material_handle.Index);
}

ImTextureID MaterialThumbnailRenderer::get_thumbnail(Origo::AssetHandle material_handle) {
	const std::uint64_t key = make_cache_key(material_handle);
	auto [it, inserted] = m_thumbnail_cache.try_emplace(key);
	if (inserted || !it->second.Buffer)
		it->second.Buffer = Origo::make_scope<Origo::FrameBuffer>(create_material_thumbnail_buffer_spec());

	if (it->second.Dirty) {
		render_thumbnail(material_handle);
		it->second.Dirty = false;
	}

	return static_cast<ImTextureID>((intptr_t)it->second.Buffer->get_color_attachment(0));
}

void MaterialThumbnailRenderer::invalidate(const Origo::UUID& material_id) {
	auto handle = Origo::AssetManager::get_instance().get_handle_by_uuid(material_id);
	if (!handle)
		return;

	const auto it = m_thumbnail_cache.find(make_cache_key(*handle));
	if (it != m_thumbnail_cache.end())
		it->second.Dirty = true;
}

void MaterialThumbnailRenderer::invalidate_all() {
	for (auto& [key, entry] : m_thumbnail_cache)
		entry.Dirty = true;
}

void MaterialThumbnailRenderer::render_thumbnail(Origo::AssetHandle material_handle) {
	auto& asset_manager = Origo::AssetManager::get_instance();
	if (!asset_manager.is_valid(material_handle))
		return;

	auto it = m_thumbnail_cache.find(make_cache_key(material_handle));
	if (it == m_thumbnail_cache.end() || !it->second.Buffer)
		return;

	GLfloat previous_clear_color[4] {};
	glGetFloatv(GL_COLOR_CLEAR_VALUE, previous_clear_color);
	glClearColor(0.165f, 0.18f, 0.205f, 1.0f);

	m_render_context.set_target(it->second.Buffer.get());
	m_render_context.set_resolve_target(nullptr);
	m_render_context.set_present_target(nullptr);
	m_render_context.set_view(create_material_thumbnail_view());
	m_render_context.set_editor_grid_enabled(false);
	m_render_context.push_directional_light(create_thumbnail_directional_light());
	m_render_context.push_point_light(create_thumbnail_rim_light());

	const Origo::AssetHandle sphere_mesh = Origo::PrimitiveShapeCache::get_instance().get_sphere_mesh();
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(-18.0f), Origo::Vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-10.0f), Origo::Vec3(1.0f, 0.0f, 0.0f));

	m_render_context.begin_frame();
	m_render_context.submit_mesh(sphere_mesh, material_handle, model);
	m_render_context.flush();
	m_render_context.end_frame();

	glClearColor(
	    previous_clear_color[0],
	    previous_clear_color[1],
	    previous_clear_color[2],
	    previous_clear_color[3]);
}

}
