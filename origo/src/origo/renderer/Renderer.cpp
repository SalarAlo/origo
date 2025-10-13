#include "origo/renderer/Renderer.h"

namespace Origo {

void Renderer::BeginFrame() {
}

void Renderer::Submit(const Ref<Mesh>& mesh, const Ref<Material>& material, const Ref<Transform>& transform) {
	m_DrawQueue.emplace_back(mesh, material, transform);
}

void Renderer::Flush() {
	std::sort(m_DrawQueue.begin(), m_DrawQueue.end(), [](const RenderCommand& a, const RenderCommand& b) {
		return a.GetMaterial() < b.GetMaterial();
	});
	Ref<Material> currentMaterial = nullptr;

	for (auto& cmd : m_DrawQueue) {
		if (cmd.GetMaterial() != currentMaterial) {
			cmd.GetMaterial()->Bind();
			currentMaterial = cmd.GetMaterial();
		}

		DrawMesh(cmd);
	}
}

void Renderer::DrawMesh(const RenderCommand& renderCommand) {
	renderCommand.GetMaterial()->WriteModel(renderCommand.GetTransform()->GetModelMatrix());
	renderCommand.GetMesh()->Render();
}

void Renderer::EndFrame() {
}

}
