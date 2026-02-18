#pragma once

#include "origo/assets/AssetManager.h"

namespace Origo {

enum class RenderPass {
	Geometry,
	Outline,
	Skybox
};

class RenderCommand {
public:
	RenderCommand(const AssetHandle& mesh, const AssetHandle& material, const glm::mat4& model, RenderPass renderPass, GLenum drawMethod)
	    : m_Mesh(mesh)
	    , m_Material(material)
	    , m_ModelMatrix(model)
	    , m_RenderPass(renderPass)
	    , m_DrawMethod(drawMethod) { }

	AssetHandle GetMesh() const { return m_Mesh; }
	AssetHandle GetMaterial() const { return m_Material; }
	glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }
	RenderPass GetRenderPass() const { return m_RenderPass; }
	GLenum GetDrawMethod() const { return m_DrawMethod; }

private:
	AssetHandle m_Mesh;
	AssetHandle m_Material;
	glm::mat4 m_ModelMatrix;
	RenderPass m_RenderPass;
	GLenum m_DrawMethod;
};
}
