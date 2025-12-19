#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Transform.h"

namespace Origo {

enum class RenderPass {
	Geometry,
	Outline,
};

class RenderCommand {
public:
	RenderCommand(const AssetHandle& mesh, const AssetHandle& material, Transform* transform, RenderPass renderPass)
	    : m_Mesh(mesh)
	    , m_Material(material)
	    , m_Transform(transform)
	    , m_RenderPass(renderPass) { }

	AssetHandle GetMesh() const { return m_Mesh; }
	AssetHandle GetMaterial() const { return m_Material; }
	Transform* GetTransform() const { return m_Transform; }
	RenderPass GetRenderPass() const { return m_RenderPass; }

private:
	AssetHandle m_Mesh;
	AssetHandle m_Material;
	Transform* m_Transform;
	RenderPass m_RenderPass;
};
}
