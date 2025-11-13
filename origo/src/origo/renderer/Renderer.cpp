#include "origo/renderer/Renderer.h"
#include "origo/core/Logger.h"

namespace Origo {

static std::vector<RenderCommand> s_DrawQueue;

static void DrawMesh(const RenderCommand& renderCommand) {
	renderCommand.GetMaterial()->WriteModel(renderCommand.GetTransform()->GetModelMatrix());
	renderCommand.GetMesh()->Render();
}

void Renderer::BeginFrame() {
}

void Renderer::Submit(Mesh* mesh, Material* material, Transform* transform) {
	s_DrawQueue.emplace_back(mesh, material, transform);
}

void Renderer::SetViewport(int width, int height) {
	glViewport(0, 0, width, height);
}

void Renderer::Flush(const Camera* camera) {
	std::sort(s_DrawQueue.begin(), s_DrawQueue.end(), [](const RenderCommand& a, const RenderCommand& b) {
		return a.GetMaterial() < b.GetMaterial();
	});

	Material* currentMaterial = nullptr;

	for (auto& cmd : s_DrawQueue) {
		if (!cmd.GetMaterial() || !cmd.GetMesh() || !cmd.GetTransform()) {
			ORG_CORE_ERROR("[Renderer] Null reference in RenderCommand");
			continue;
		}

		if (cmd.GetMaterial() != currentMaterial) {
			cmd.GetMaterial()->Bind();
			currentMaterial
			    = cmd.GetMaterial();

			currentMaterial
			    ->SetShaderDirectly("u_ProjectionMatrix", camera->GetProjection())
			    .SetShaderDirectly("u_ViewMatrix", camera->GetView())
			    .SetShaderDirectly("u_CameraForward", camera->GetForward())
			    .SetShaderDirectly("u_ViewPos", camera->GetPosition());
		}

		DrawMesh(cmd);
	}

	s_DrawQueue.clear();
}

void Renderer::EndFrame() {
}

}
