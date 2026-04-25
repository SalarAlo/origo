#include "panels/GeometryThumbnailRenderer.h"

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/Mesh.h"
#include "origo/assets/model/Model.h"

#include "origo/components/DirectionalLightData.h"
#include "origo/components/PointLightData.h"

#include "origo/renderer/RenderView.h"

namespace {

constexpr int k_thumbnail_size = 128;
constexpr float k_thumbnail_fov_degrees = 30.0f;

Origo::FrameBufferSpec create_geometry_thumbnail_buffer_spec() {
	Origo::FrameBufferSpec spec;
	spec.Width = k_thumbnail_size;
	spec.Height = k_thumbnail_size;
	spec.Attachments = {
		{ Origo::AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
		{ Origo::AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
	};
	return spec;
}

Origo::RenderView create_geometry_thumbnail_view(const glm::vec3& target, float radius) {
	const glm::vec3 view_direction = glm::normalize(glm::vec3(0.62f, 0.42f, 0.66f));
	const float safe_radius = std::max(radius, 0.35f);
	const float half_fov = glm::radians(k_thumbnail_fov_degrees * 0.5f);
	const float distance = std::max(safe_radius / std::tan(half_fov), safe_radius * 2.4f);
	const Origo::Vec3 eye = target + view_direction * distance;
	const Origo::Vec3 up { 0.0f, 1.0f, 0.0f };

	Origo::RenderView view {};
	view.Projection = glm::perspective(glm::radians(k_thumbnail_fov_degrees), 1.0f, 0.1f, 100.0f);
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

glm::mat4 create_mesh_thumbnail_transform() {
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(-28.0f), Origo::Vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-14.0f), Origo::Vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, Origo::Vec3(1.12f));
	return model;
}

glm::mat4 create_model_thumbnail_transform() {
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), Origo::Vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, Origo::Vec3(1.05f));
	return model;
}

glm::vec3 transform_point(const glm::mat4& transform, const glm::vec3& point) {
	return glm::vec3(transform * glm::vec4(point, 1.0f));
}

glm::vec3 compute_target_center(const glm::vec3& bounds_min, const glm::vec3& bounds_max, const glm::mat4& model_matrix) {
	return transform_point(model_matrix, (bounds_min + bounds_max) * 0.5f);
}

float compute_target_radius(const glm::vec3& bounds_min, const glm::vec3& bounds_max, const glm::mat4& model_matrix) {
	const glm::vec3 extents = (bounds_max - bounds_min) * 0.5f;
	const float scale_x = glm::length(glm::vec3(model_matrix[0]));
	const float scale_y = glm::length(glm::vec3(model_matrix[1]));
	const float scale_z = glm::length(glm::vec3(model_matrix[2]));
	const float max_scale = std::max({ scale_x, scale_y, scale_z, 0.001f });
	return glm::length(extents) * max_scale;
}

}

namespace OrigoEditor {

GeometryThumbnailRenderer& GeometryThumbnailRenderer::get_instance() {
	static GeometryThumbnailRenderer instance;
	return instance;
}

GeometryThumbnailRenderer::GeometryThumbnailRenderer()
    : m_render_context(nullptr) {
}

std::uint64_t GeometryThumbnailRenderer::make_cache_key(Origo::AssetHandle asset_handle, PreviewType preview_type) {
	const std::uint64_t handle_bits = (static_cast<std::uint64_t>(asset_handle.Generation) << 32)
	    | static_cast<std::uint64_t>(asset_handle.Index);
	return (handle_bits << 1) | static_cast<std::uint64_t>(preview_type == PreviewType::Model);
}

ImTextureID GeometryThumbnailRenderer::get_mesh_thumbnail(Origo::AssetHandle mesh_handle) {
	return get_thumbnail(mesh_handle, PreviewType::Mesh);
}

ImTextureID GeometryThumbnailRenderer::get_model_thumbnail(Origo::AssetHandle model_handle) {
	return get_thumbnail(model_handle, PreviewType::Model);
}

void GeometryThumbnailRenderer::invalidate(const Origo::UUID& asset_id) {
	auto handle = Origo::AssetManager::get_instance().get_handle_by_uuid(asset_id);
	if (!handle)
		return;

	for (PreviewType preview_type : { PreviewType::Mesh, PreviewType::Model }) {
		const auto it = m_thumbnail_cache.find(make_cache_key(*handle, preview_type));
		if (it != m_thumbnail_cache.end())
			it->second.Dirty = true;
	}
}

void GeometryThumbnailRenderer::invalidate_all() {
	for (auto& [key, entry] : m_thumbnail_cache)
		entry.Dirty = true;
}

ImTextureID GeometryThumbnailRenderer::get_thumbnail(Origo::AssetHandle asset_handle, PreviewType preview_type) {
	const std::uint64_t key = make_cache_key(asset_handle, preview_type);
	auto [it, inserted] = m_thumbnail_cache.try_emplace(key);
	if (inserted || !it->second.Buffer)
		it->second.Buffer = Origo::make_scope<Origo::FrameBuffer>(create_geometry_thumbnail_buffer_spec());

	if (it->second.Dirty) {
		render_thumbnail(asset_handle, preview_type);
		it->second.Dirty = false;
	}

	return static_cast<ImTextureID>((intptr_t)it->second.Buffer->get_color_attachment(0));
}

void GeometryThumbnailRenderer::render_thumbnail(Origo::AssetHandle asset_handle, PreviewType preview_type) {
	auto& asset_manager = Origo::AssetManager::get_instance();
	if (!asset_manager.is_valid(asset_handle))
		return;

	if (preview_type == PreviewType::Model) {
		auto* model = asset_manager.get_asset<Origo::Model>(asset_handle);
		if (!model)
			return;
		if (model->get_root_node() < 0 || model->get_nodes().empty() || model->get_sub_meshes().empty())
			return;
	}

	auto it = m_thumbnail_cache.find(make_cache_key(asset_handle, preview_type));
	if (it == m_thumbnail_cache.end() || !it->second.Buffer)
		return;

	GLfloat previous_clear_color[4] {};
	glGetFloatv(GL_COLOR_CLEAR_VALUE, previous_clear_color);
	glClearColor(0.145f, 0.155f, 0.175f, 1.0f);

	m_render_context.set_target(it->second.Buffer.get());
	m_render_context.set_resolve_target(nullptr);
	m_render_context.set_present_target(nullptr);
	m_render_context.set_editor_grid_enabled(false);
	m_render_context.push_directional_light(create_thumbnail_directional_light());
	m_render_context.push_point_light(create_thumbnail_rim_light());

	const glm::mat4 model_matrix = preview_type == PreviewType::Model
	    ? create_model_thumbnail_transform()
	    : create_mesh_thumbnail_transform();
	const glm::vec3 bounds_min = preview_type == PreviewType::Model
	    ? asset_manager.get_asset<Origo::Model>(asset_handle)->get_bounds_min()
	    : asset_manager.get_asset<Origo::Mesh>(asset_handle)->BoundsMin;
	const glm::vec3 bounds_max = preview_type == PreviewType::Model
	    ? asset_manager.get_asset<Origo::Model>(asset_handle)->get_bounds_max()
	    : asset_manager.get_asset<Origo::Mesh>(asset_handle)->BoundsMax;
	const glm::vec3 target_center = compute_target_center(bounds_min, bounds_max, model_matrix);
	const float target_radius = compute_target_radius(bounds_min, bounds_max, model_matrix);
	m_render_context.set_view(create_geometry_thumbnail_view(target_center, target_radius));

	m_render_context.begin_frame();
	if (preview_type == PreviewType::Model) {
		m_render_context.submit_model(asset_handle, model_matrix);
	} else {
		m_render_context.submit_mesh(asset_handle, Origo::DefaultAssetCache::get_instance().get_material(), model_matrix);
	}
	m_render_context.flush();
	m_render_context.end_frame();

	glClearColor(
	    previous_clear_color[0],
	    previous_clear_color[1],
	    previous_clear_color[2],
	    previous_clear_color[3]);
}

}
