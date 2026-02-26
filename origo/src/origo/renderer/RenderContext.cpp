#include "origo/renderer/RenderContext.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/Material2D.h"
#include "origo/assets/Mesh.h"
#include "origo/assets/Model.h"
#include "origo/assets/SkyboxMaterial.h"

#include "origo/components/DirectionalLightData.h"

#include "origo/renderer/GlDebug.h"
#include "origo/renderer/RenderCommand.h"
#include "origo/renderer/VaoCache.h"

namespace Origo {

static void draw_mesh(const RenderCommand& cmd) {
	auto& am { AssetManager::get_instance() };
	auto material { am.get_asset<Material2D>(cmd.get_material()) };
	auto mesh { am.get_asset<Mesh>(cmd.get_mesh()) };

	constexpr float outline_thickness { 0.1f };
	glm::mat4 model = cmd.get_render_pass() == RenderPass::Skybox ? glm::mat4(1.0f) : cmd.get_model_matrix();
	if (cmd.get_render_pass() == RenderPass::Outline)
		model = glm::scale(model, Vec3(1.0f + outline_thickness));

	material->write_model(model);

	const VaoCache::Entry& vao_entry = VaoCache::create_or_get(mesh->LayoutId, mesh->HeapId);

	GLCall(glBindVertexArray(vao_entry.VAO));

	const MeshRange& r = mesh->Range;
	GLCall(glDrawElementsBaseVertex(
	    cmd.get_draw_method(),
	    r.IndexCount,
	    GL_UNSIGNED_INT,
	    reinterpret_cast<void*>(r.FirstIndex * sizeof(unsigned int)),
	    r.FirstVertex));
}

void RenderContext::begin_frame() {
	if (!m_has_view) {
		throw std::runtime_error("RenderContext::Flush: called without a view (SetView not called).");
	}

	if (m_skybox_vao == 0) {
		glGenVertexArrays(1, &m_skybox_vao);
	}

	FrameBuffer* fb = m_target;
	if (!fb)
		return;
	glViewport(0, 0, fb->get_width(), fb->get_height());
}

void RenderContext::set_view(const RenderView& view) {
	m_view = view;
	m_has_view = true;
}

void RenderContext::submit_mesh(const AssetHandle& mesh, const AssetHandle& material, const glm::mat4& modelMatrix, RenderPass pass, GLenum drawMethod) {
	m_draw_queue.emplace_back(mesh, material, modelMatrix, pass, drawMethod);
}

void RenderContext::flush() {
	bind_fb();
	clear();

	execute_pass(RenderPass::Skybox);
	execute_pass(RenderPass::Geometry);
	execute_pass(RenderPass::Outline);
}

void RenderContext::end_frame() {
	m_target->unbind();

	if (m_target->is_msaa()) {
		if (!m_resolve_target)
			throw std::runtime_error("MSAA target requires a resolve target");

		m_target->resolve_to(*m_resolve_target);
	}

	m_directional_light_data.reset();
	m_point_lights.clear();
	m_draw_queue.clear();
}

void RenderContext::clear() {
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}
void RenderContext::bind_fb() {
	FrameBuffer* fb = m_target;
	if (!fb)
		return;

	fb->bind();
}

void RenderContext::execute_pass(RenderPass pass) {
	configure_state(pass);

	if (pass == RenderPass::Skybox) {
		if (!m_skybox_material.has_value())
			return;

		auto material = AssetManager::get_instance().get_asset<SkyboxMaterial>(*m_skybox_material);
		material->bind(m_view.Projection, m_view.View);

		glBindVertexArray(m_skybox_vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		return;
	}

	Material2D* current_material {};
	OptionalAssetHandle current_material_id {};

	for (auto& cmd : m_draw_queue) {
		if (cmd.get_render_pass() != pass)
			continue;

		if (!current_material_id.has_value() || cmd.get_material() != current_material_id) {
			auto material { AssetManager::get_instance().get_asset<Material2D>(cmd.get_material()) };
			material->bind();

			current_material = material;
			current_material_id = cmd.get_material();

			current_material
			    ->set_shader_directly("u_ProjectionMatrix", m_view.Projection)
			    .set_shader_directly("u_ViewMatrix", m_view.View)
			    .set_shader_directly("u_CameraForward", m_view.CameraForward)
			    .set_shader_directly("u_ViewPos", m_view.CameraPosition);

			if (!m_directional_light_data)
				m_directional_light_data = DirectionalLightData {};

			auto l { *m_directional_light_data };

			current_material
			    ->set_shader_directly("u_DirLight.direction", l.Direction)
			    .set_shader_directly("u_DirLight.color", l.Color)
			    .set_shader_directly("u_DirLight.intensity", l.Intensity)
			    .set_shader_directly("u_Ambient", l.Ambient);

			const int count = std::min<int>(m_point_lights.size(), 8);

			current_material->set_shader_directly("u_PointLightCount", count);

			for (int i = 0; i < count; ++i) {
				const auto& l = m_point_lights[i];
				const std::string base = "u_PointLights[" + std::to_string(i) + "]";

				current_material
				    ->set_shader_directly(base + ".position", l.Position)
				    .set_shader_directly(base + ".color", l.Color)
				    .set_shader_directly(base + ".intensity", l.Intensity)
				    .set_shader_directly(base + ".constant", l.Constant)
				    .set_shader_directly(base + ".linear", l.Linear)
				    .set_shader_directly(base + ".quadratic", l.Quadratic);
			}
		}

		draw_mesh(cmd);
	}
}

void RenderContext::push_directional_light(const DirectionalLightData& directionalLightData) {
	m_directional_light_data = directionalLightData;
}

void RenderContext::push_point_light(const PointLightData& data) {
	m_point_lights.push_back(data);
}

void RenderContext::configure_state(RenderPass pass) {
	switch (pass) {
	case RenderPass::Geometry: {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_STENCIL_TEST);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glDepthMask(GL_TRUE);

		break;
	}

	case RenderPass::Outline: {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDepthMask(GL_TRUE);

		break;
	}

	case RenderPass::Skybox: {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_CULL_FACE);
		break;
	}
	}
}

void RenderContext::submit_model(const AssetHandle& modelHandle, const glm::mat4& modelMatrix, RenderPass pass, const std::optional<AssetHandle>& material) {
	auto model = AssetManager::get_instance().get_asset<Model>(modelHandle);
	if (!model)
		return;

	const auto& nodes = model->get_nodes();
	const auto& sub_meshes = model->get_sub_meshes();

	std::vector<glm::mat4> world_matrices(nodes.size());

	world_matrices[model->get_root_node()] = modelMatrix * nodes[model->get_root_node()].LocalTransform;

	for (size_t i = 0; i < nodes.size(); ++i) {
		const auto& node = nodes[i];

		if (node.Parent != -1) {
			world_matrices[i] = world_matrices[node.Parent] * node.LocalTransform;
		}

		if (node.SubMeshIndex != -1) {
			const auto& sub = sub_meshes[node.SubMeshIndex];

			submit_mesh(
			    sub.Mesh,
			    material.has_value() ? *material : sub.Material,
			    world_matrices[i],
			    pass);
		}
	}
}

void RenderContext::set_skybox_material(AssetHandle skyboxMaterial) {
	m_skybox_material = skyboxMaterial;
}
}
