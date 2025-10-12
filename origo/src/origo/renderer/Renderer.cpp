#include "origo/renderer/Renderer.h"
#include "origo/renderer/Mesh.h"
#include "origo/renderer/Material.h"
#include "origo/renderer/Shader.h"

namespace Origo {

void Renderer::BeginFrame() {
}

void Renderer::Submit(const Renderable& renderable, const glm::mat4& transform) {
}

void Renderer::Flush() {
}

void Renderer::EndFrame() {
	// TODO: Swap buffers or finalize the frame
}

}
