#pragma once

#include "Batch.h"
#include "RenderCommand.h"
#include <glm/glm.hpp>

namespace Origo {

class Renderer {
public:
	void BeginFrame();
	void Submit(const RenderCommand& renderable, const glm::mat4& transform);
	void Flush();
	void EndFrame();

private:
	std::unordered_map<Ref<Material>, Batch> m_Commands {};
};

}
