#pragma once

#include "Renderable.h"
#include "Batch.h"
#include <glm/glm.hpp>

namespace Origo {

class Renderer {
public:
	void BeginFrame();
	void Submit(const Renderable& renderable, const glm::mat4& transform);
	void Flush();
	void EndFrame();

private:
	std::unordered_map<Material*, Batch> m_Commands;
};

}
