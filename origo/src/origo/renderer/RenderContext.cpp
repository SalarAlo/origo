#include "origo/renderer/RenderContext.h"

namespace Origo {

static void DrawMesh(const RenderCommand& renderCommand) {
	renderCommand.GetMaterial()->WriteModel(renderCommand.GetTransform()->GetModelMatrix());
	renderCommand.GetMesh()->Render();
}

void RenderContext::BeginFrame() {
	glViewport(0, 0, m_Buffer->GetWidth(), m_Buffer->GetHeight());
}

void RenderContext::Submit(Mesh* mesh, Material* material, Transform* transform) {
	m_DrawQueue.emplace_back(mesh, material, transform);
}

void RenderContext::Flush(Camera* camera) {

	camera->SetAspectResolution(static_cast<float>(m_Buffer->GetWidth()) / m_Buffer->GetHeight());

	std::sort(m_DrawQueue.begin(), m_DrawQueue.end(), [](const RenderCommand& a, const RenderCommand& b) {
		return a.GetMaterial()->GetId().ToString() < b.GetMaterial()->GetId().ToString();
	});

	Material* currentMaterial = nullptr;

	m_Buffer->Bind();

	GLCall(glClearColor(0.1f, 0.1f, 0.12f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (auto& cmd : m_DrawQueue) {
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

	m_Buffer->Unbind();

	m_DrawQueue.clear();
}

void RenderContext::EndFrame() {
	return;
}
}
