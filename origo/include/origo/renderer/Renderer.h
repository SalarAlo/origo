#pragma once

#include "RenderCommand.h"
#include "origo/renderer/Material.h"
#include "origo/renderer/Transform.h"
#include <glm/glm.hpp>

namespace Origo {

class Renderer {
public:
	void BeginFrame();
	void Submit(const Ref<Mesh>& mesh, const Ref<Material>& material, const Ref<Transform>& transform);
	void Flush();
	void EndFrame();

private:
	void DrawMesh(const RenderCommand& renderCommand);

private:
	std::vector<RenderCommand> m_DrawQueue;
};

}
