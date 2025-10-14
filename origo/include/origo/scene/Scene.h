#pragma once

#include "origo/Camera.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/Entity.hpp"

namespace Origo {

class Scene {
public:
	Ref<Entity> CreateEntity();
	void Render();

private:
	std::vector<Ref<Entity>> m_Entities {};
	Camera m_Camera {};
	Origo::Renderer m_Renderer {};
};

}
