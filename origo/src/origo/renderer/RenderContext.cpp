#include "origo/renderer/RenderContext.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Material.h"
#include "origo/renderer/GlDebug.h"
#include "origo/assets/Mesh.h"
#include "origo/renderer/RenderCommand.h"
#include "origo/renderer/VaoCache.h"

namespace Origo {

static void DrawMesh(const RenderCommand& cmd, GLenum drawMethod) {
	auto& am = AssetManagerFast::GetInstance();
	auto material = am.Get<Material>(cmd.GetMaterial());
	auto mesh = am.Get<Mesh>(cmd.GetMesh());

	constexpr float outlineThickness { 0.03f };
	glm::mat4 model = cmd.GetTransform()->GetModelMatrix();
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

	FrameBuffer* fb = m_Target;
	if (!fb)
		return;
	glViewport(0, 0, fb->GetWidth(), fb->GetHeight());
}

void RenderContext::Submit(const AssetHandle& mesh, const AssetHandle& material, Transform* transform, RenderPass pass) {
	m_DrawQueue.emplace_back(mesh, material, transform, pass);
}

void RenderContext::Flush() {
	BindFB();
	Clear();

	ExecutePass(RenderPass::Geometry);
	ExecutePass(RenderPass::Outline);

	Resolve();
}

void RenderContext::EndFrame() {
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

	Material* currentMaterial {};
	AssetHandle currentMaterialId {};

	for (auto& cmd : m_DrawQueue) {
		if (cmd.GetRenderPass() != pass)
			continue;

		if (cmd.GetMaterial() != currentMaterialId) {
			auto material { AssetManagerFast::GetInstance().Get<Material>(cmd.GetMaterial()) };
			material->Bind();

			currentMaterial = material;
			currentMaterialId = cmd.GetMaterial();

			currentMaterial
			    ->SetShaderDirectly("u_ProjectionMatrix", m_View.Projection)
			    .SetShaderDirectly("u_ViewMatrix", m_View.View)
			    .SetShaderDirectly("u_CameraForward", m_View.CameraForward)
			    .SetShaderDirectly("u_ViewPos", m_View.CameraPosition);
		}

		DrawMesh(cmd, m_DrawMethod);
	}
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
	}
}

void RenderContext::Resolve() {
	m_Target->Unbind();

	if (m_Target->IsMSAA()) {
		if (!m_ResolveTarget)
			throw std::runtime_error("MSAA target requires a resolve target");

		m_Target->ResolveTo(*m_ResolveTarget);
	}

	m_DrawQueue.clear();
}

}
