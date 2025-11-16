#pragma once

#include "RenderCommand.h"
#include "origo/Camera.h"
#include "origo/assets/Material.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/Transform.h"

#include <glm/glm.hpp>

namespace Origo::Renderer {

void SetTarget(FrameBuffer* buffer);
void BeginFrame();
void Submit(Mesh* mesh, Material* material, Transform* transform);
void SetViewport(int width, int height);
void Flush(Camera* camera);
void EndFrame();

}
