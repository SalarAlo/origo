#pragma once

#include "origo/assets/AssetManager.h"

namespace Origo {

enum class RenderPass {
	Geometry,
	Grid,
	Outline,
	Skybox
};

class RenderCommand {
public:
	RenderCommand(const AssetHandle& mesh, const AssetHandle& material, const glm::mat4& model, RenderPass renderPass, GLenum drawMethod, int entityId = -1)
	    : m_mesh(mesh)
	    , m_material(material)
	    , m_model_matrix(model)
	    , m_render_pass(renderPass)
	    , m_draw_method(drawMethod)
	    , m_entity_id(entityId) { }

	AssetHandle get_mesh() const { return m_mesh; }
	AssetHandle get_material() const { return m_material; }
	glm::mat4 get_model_matrix() const { return m_model_matrix; }
	RenderPass get_render_pass() const { return m_render_pass; }
	GLenum get_draw_method() const { return m_draw_method; }
	int get_entity_id() const { return m_entity_id; }

private:
	AssetHandle m_mesh;
	AssetHandle m_material;
	glm::mat4 m_model_matrix;
	RenderPass m_render_pass;
	GLenum m_draw_method;
	int m_entity_id;
};
}
