#pragma once

#include "RenderCommand.h"
#include "origo/Camera.h"
#include "origo/renderer/Material.h"
#include "origo/renderer/Transform.h"
#include <glm/glm.hpp>

namespace Origo {

class Renderer {
public:
	void BeginFrame();
	void Submit(Ref<Mesh> mesh, Ref<Material> material, Ref<Transform> transform);
	void Flush(const Camera& camera);
	void EndFrame();

private:
	void DrawMesh(const RenderCommand& renderCommand);

private:
	std::vector<RenderCommand> m_DrawQueue;
};

}
