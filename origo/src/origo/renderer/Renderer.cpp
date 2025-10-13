#include "origo/renderer/Renderer.h"

namespace Origo {

void Renderer::BeginFrame() {
}

void Renderer::Submit(const RenderCommand& renderable, const glm::mat4& transform) {
	auto material = renderable.GetMaterial();
	auto mesh = renderable.GetMesh();

	auto it = m_Commands.find(material);
	if (it != m_Commands.end()) {
		it->second.AddMesh(mesh);
	} else {
		Batch batch {};
		batch.AddMesh(mesh);
		m_Commands.emplace(material, std::move(batch));
	}
}

void Renderer::Flush() {
	for (const auto& [material, batch] : m_Commands) {
		material->Bind();
		batch.Render();
	}
}

void Renderer::EndFrame() {
}

}
