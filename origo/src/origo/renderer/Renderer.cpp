#include "origo/renderer/Renderer.h"

namespace Origo {

void Renderer::BeginFrame() {
}

void Renderer::Submit(Ref<Mesh> mesh, Ref<Material> material, Ref<Transform> transform) {
	m_DrawQueue.emplace_back(mesh, material, transform);
}

void Renderer::Flush(const Camera& camera) {
	std::sort(m_DrawQueue.begin(), m_DrawQueue.end(), [](const RenderCommand& a, const RenderCommand& b) {
		return a.GetMaterial() < b.GetMaterial();
	});

	Ref<Material> currentMaterial = nullptr;

	for (auto& cmd : m_DrawQueue) {

		if (!cmd.GetMaterial() || !cmd.GetMesh() || !cmd.GetTransform()) {
			std::cerr << "[Renderer] Null reference in RenderCommand\n";
			continue;
		}

		if (cmd.GetMaterial() != currentMaterial) {
			cmd.GetMaterial()->Bind();
			currentMaterial = cmd.GetMaterial();

			currentMaterial
			    ->Set("u_ProjectionMatrix", camera.GetProjection())
			    .Set("u_ViewMatrix", camera.GetView())
			    .Set("u_ViewPos", camera.GetPosition());
		}

		DrawMesh(cmd);
	}

	m_DrawQueue.clear();
}

void Renderer::DrawMesh(const RenderCommand& renderCommand) {
	renderCommand.GetMaterial()->WriteModel(renderCommand.GetTransform()->GetModelMatrix());
	renderCommand.GetMesh()->Render();
}

void Renderer::EndFrame() {
}

}
