#include "origo/renderer/RenderContext.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Material2D.h"
#include "origo/assets/SkyboxMaterial.h"
#include "origo/components/DirectionalLightData.h"
#include "origo/renderer/GlDebug.h"
#include "origo/assets/Mesh.h"
#include "origo/renderer/RenderCommand.h"
#include "origo/renderer/VaoCache.h"

namespace Origo {

static void DrawMesh(const RenderCommand& cmd, GLenum drawMethod) {
	auto& am { AssetManager::GetInstance() };
	auto material { am.Get<Material2D>(cmd.GetMaterial()) };
	auto mesh { am.Get<Mesh>(cmd.GetMesh()) };

	constexpr float outlineThickness { 0.1f };
	glm::mat4 model = cmd.GetRenderPass() == RenderPass::Skybox ? glm::mat4(1.0f) : cmd.GetModelMatrix();
	if (cmd.GetRenderPass() == RenderPass::Outline)
		model = glm::scale(model, glm::vec3(1.0f + outlineThickness));

	material->WriteModel(model);

	const VaoCache::Entry& vaoEntry = VaoCache::CreateOrGet(mesh->LayoutId, mesh->HeapId);

	GLCall(glBindVertexArray(vaoEntry.VAO));

	const MeshRange& r = mesh->Range;
	GLCall(glDrawElementsBaseVertex(
	    drawMethod,
	    r.IndexCount,
	    GL_UNSIGNED_INT,
	    reinterpret_cast<void*>(r.FirstIndex * sizeof(unsigned int)),
	    r.FirstVertex));

	GLCall(glBindVertexArray(0));
}

void RenderContext::BeginFrame() {
	if (!m_HasView) {
		throw std::runtime_error("RenderContext::Flush: called without a view (SetView not called).");
	}

	if (m_SkyboxVAO == 0) {
		glGenVertexArrays(1, &m_SkyboxVAO);
	}

	FrameBuffer* fb = m_Target;
	if (!fb)
		return;
	glViewport(0, 0, fb->GetWidth(), fb->GetHeight());
}

void RenderContext::Submit(const AssetHandle& mesh, const AssetHandle& material, const glm::mat4& modelMatrix, RenderPass pass) {
	m_DrawQueue.emplace_back(mesh, material, modelMatrix, pass);
}

void RenderContext::Flush() {
	BindFB();
	Clear();

	ExecutePass(RenderPass::Skybox);
	ExecutePass(RenderPass::Geometry);
	ExecutePass(RenderPass::Outline);
}

void RenderContext::EndFrame() {
	m_Target->Unbind();

	if (m_Target->IsMSAA()) {
		if (!m_ResolveTarget)
			throw std::runtime_error("MSAA target requires a resolve target");

		m_Target->ResolveTo(*m_ResolveTarget);
	}

	m_DirectionalLightData.reset();
	m_PointLights.clear();
	m_DrawQueue.clear();
}

void RenderContext::Clear() {
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}
void RenderContext::BindFB() {
	FrameBuffer* fb = m_Target;
	if (!fb)
		return;

	fb->Bind();
}

void RenderContext::ExecutePass(RenderPass pass) {
	ConfigureState(pass);

	if (pass == RenderPass::Skybox) {
		if (!m_SkyboxMaterial.has_value())
			return;

		auto material = AssetManager::GetInstance().Get<SkyboxMaterial>(*m_SkyboxMaterial);
		material->Bind(m_View.Projection, m_View.View);

		glBindVertexArray(m_SkyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		return;
	}

	Material2D* currentMaterial {};
	OptionalAssetHandle currentMaterialId {};

	for (auto& cmd : m_DrawQueue) {
		if (cmd.GetRenderPass() != pass)
			continue;

		if (!currentMaterialId.has_value() || cmd.GetMaterial() != currentMaterialId) {
			auto material { AssetManager::GetInstance().Get<Material2D>(cmd.GetMaterial()) };
			material->Bind();

			currentMaterial = material;
			currentMaterialId = cmd.GetMaterial();

			currentMaterial
			    ->SetShaderDirectly("u_ProjectionMatrix", m_View.Projection)
			    .SetShaderDirectly("u_ViewMatrix", m_View.View)
			    .SetShaderDirectly("u_CameraForward", m_View.CameraForward)
			    .SetShaderDirectly("u_ViewPos", m_View.CameraPosition);

			if (!m_DirectionalLightData)
				m_DirectionalLightData = DirectionalLightData {};

			auto l { *m_DirectionalLightData };

			currentMaterial
			    ->SetShaderDirectly("u_DirLight.direction", l.Direction)
			    .SetShaderDirectly("u_DirLight.color", l.Color)
			    .SetShaderDirectly("u_DirLight.intensity", l.Intensity)
			    .SetShaderDirectly("u_Ambient", l.Ambient);

			const int count = std::min<int>(m_PointLights.size(), 8);

			currentMaterial->SetShaderDirectly("u_PointLightCount", count);

			for (int i = 0; i < count; ++i) {
				const auto& l = m_PointLights[i];
				const std::string base = "u_PointLights[" + std::to_string(i) + "]";

				currentMaterial
				    ->SetShaderDirectly(base + ".position", l.position)
				    .SetShaderDirectly(base + ".color", l.color)
				    .SetShaderDirectly(base + ".intensity", l.intensity)
				    .SetShaderDirectly(base + ".constant", l.constant)
				    .SetShaderDirectly(base + ".linear", l.linear)
				    .SetShaderDirectly(base + ".quadratic", l.quadratic);
			}
		}

		DrawMesh(cmd, m_DrawMethod);
	}
}

void RenderContext::PushDirectionalLight(const DirectionalLightData& directionalLightData) {
	m_DirectionalLightData = directionalLightData;
}

void RenderContext::PushPointLight(const PointLightData& data) {
	m_PointLights.push_back(data);
}

void RenderContext::ConfigureState(RenderPass pass) {
	switch (pass) {
	case RenderPass::Geometry: {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_STENCIL_TEST);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_ZERO, GL_REPLACE);
		glDepthMask(GL_TRUE);

		break;
	}

	case RenderPass::Outline: {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDepthMask(GL_TRUE);

		break;
	}

	case RenderPass::Skybox: {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_CULL_FACE);
		break;
	}
	}
}

void RenderContext::SetSkyboxMaterial(AssetHandle skyboxMaterial) {
	m_SkyboxMaterial = skyboxMaterial;
}

}
