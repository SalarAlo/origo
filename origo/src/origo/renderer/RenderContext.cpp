#include "origo/renderer/RenderContext.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Material.h"
#include "origo/renderer/GeometryHeap.h"
#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/GlDebug.h"
#include "origo/assets/Mesh.h"
#include "origo/renderer/VaoCache.h"

namespace Origo {
static std::hash<UUID> HashEntity {};

static void DrawMesh(const RenderCommand& cmd, GLenum drawMethod) {
	auto& am { AssetManagerFast::GetInstance() };
	auto material = am.Get<Material>(cmd.GetMaterial());
	auto mesh = am.Get<Mesh>(cmd.GetMesh());
	auto shader = am.Get<Shader>(material->GetShader());

	material->WriteModel(cmd.GetTransform()->GetModelMatrix());

	GeometryHeap* heap = GeometryHeapRegistry::GetHeap(mesh->HeapId);

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, heap->GetIbo()));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, heap->GetVbo()));

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
	FrameBuffer* fb = m_Target;
	if (!fb)
		return;
	glViewport(0, 0, fb->GetWidth(), fb->GetHeight());
}

void RenderContext::Submit(const AssetHandle& mesh, const AssetHandle& material, Transform* transform) {
	m_DrawQueue.emplace_back(mesh, material, transform);
}

void RenderContext::Flush(Camera* camera) {

	FrameBuffer* fb = m_Target;
	if (!fb)
		return;

	camera->SetAspectResolution(static_cast<float>(fb->GetWidth()) / fb->GetHeight());

	Material* currentMaterial {};
	AssetHandle currentMaterialId {};

	fb->Bind();

	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (auto& cmd : m_DrawQueue) {
		if (cmd.GetMaterial() != currentMaterialId) {
			auto material { AssetManagerFast::GetInstance().Get<Material>(cmd.GetMaterial()) };
			material->Bind();

			currentMaterial = material;
			currentMaterialId = cmd.GetMaterial();

			currentMaterial
			    ->SetShaderDirectly("u_ProjectionMatrix", camera->GetProjection())
			    .SetShaderDirectly("u_ViewMatrix", camera->GetView())
			    .SetShaderDirectly("u_CameraForward", camera->GetForward())
			    .SetShaderDirectly("u_ViewPos", camera->GetPosition());
		}

		DrawMesh(cmd, m_DrawMethod);
	}

	fb->Unbind();

	if (fb->GetSamples() > 1) {
		if (!m_ResolveTarget)
			throw std::runtime_error("MSAA target requires a resolve target");
		fb->ResolveTo(*m_ResolveTarget);
	}

	m_DrawQueue.clear();
}

void RenderContext::EndFrame() {
}

}
