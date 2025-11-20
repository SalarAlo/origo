#include "origo/renderer/RenderContext.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Material.h"
#include "origo/assets/Mesh.h"

namespace Origo {

static void DrawMesh(const RenderCommand& renderCommand) {
	auto material { AssetManager::GetAssetAs<Material>(renderCommand.GetMaterial()) };
	auto mesh { AssetManager::GetAssetAs<Mesh>(renderCommand.GetMesh()) };

	material->WriteModel(renderCommand.GetTransform()->GetModelMatrix());
	mesh->Render();
}

void RenderContext::BeginFrame() {
	glViewport(0, 0, m_Buffer->GetWidth(), m_Buffer->GetHeight());
}

void RenderContext::Submit(const RID& mesh, const RID& material, Transform* transform) {
	m_DrawQueue.emplace_back(mesh, material, transform);
}

void RenderContext::Flush(Camera* camera) {

	camera->SetAspectResolution(static_cast<float>(m_Buffer->GetWidth()) / m_Buffer->GetHeight());

	std::sort(m_DrawQueue.begin(), m_DrawQueue.end(), [](const RenderCommand& a, const RenderCommand& b) {
		return a.GetMaterial().GetId() < b.GetMaterial().GetId();
	});

	Material* currentMaterial {};
	RID currentMaterialId {};

	m_Buffer->Bind();

	GLCall(glClearColor(0.1f, 0.1f, 0.12f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (auto& cmd : m_DrawQueue) {
		if (cmd.GetMaterial() != currentMaterialId) {
			auto material { AssetManager::GetAssetAs<Material>(cmd.GetMaterial()) };
			material->Bind();

			currentMaterial = material;
			currentMaterialId = cmd.GetMaterial();

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
