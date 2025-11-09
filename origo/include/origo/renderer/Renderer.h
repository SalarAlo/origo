#pragma once

#include "RenderCommand.h"
#include "origo/Camera.h"
#include "origo/assets/Material.h"
#include "origo/scene/Transform.h"

#include <glm/glm.hpp>

namespace Origo::Renderer {

void BeginFrame();
void Submit(Mesh* mesh, Material* material, Transform* transform);
void SetViewport(int width, int height);
void Flush(const Camera* camera);
void EndFrame();

}
