#include "origo/renderer/RenderContext.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Material.h"
#include "origo/renderer/GeometryHeap.h"
#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/GlDebug.h"
#include "origo/assets/Mesh.h"
#include "origo/renderer/VaoCache.h"

namespace Origo {
static std::hash<UUID> HashEntity {};

static void DrawMesh(const RenderCommand& cmd, GLenum drawMethod) {
	auto material = AssetManager::Get<Material>(cmd.GetMaterial());
	auto mesh = AssetManager::Get<Mesh>(cmd.GetMesh());
	auto shader = AssetManager::Get<Shader>(material->GetShader());

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
	glViewport(0, 0, m_Buffer->GetWidth(), m_Buffer->GetHeight());
}

void RenderContext::Submit(const UUID& mesh, const UUID& material, Transform* transform) {
	m_DrawQueue.emplace_back(mesh, material, transform);
}

void RenderContext::Flush(Camera* camera) {
	camera->SetAspectResolution(static_cast<float>(m_Buffer->GetWidth()) / m_Buffer->GetHeight());

	// CRAZY |
	// std::sort(m_DrawQueue.begin(), m_DrawQueue.end(), [](const RenderCommand& a, const RenderCommand& b) {
	//	return a.GetMaterial().ToString() < b.GetMaterial().ToString();
	// });

	Material* currentMaterial {};
	UUID currentMaterialId { UUID::Bad() };

	m_Buffer->Bind();

	// GLCall(glClearColor(0.1f, 0.1f, 0.12f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (auto& cmd : m_DrawQueue) {
		if (cmd.GetMaterial() != currentMaterialId) {
			auto material { AssetManager::Get<Material>(cmd.GetMaterial()) };
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

	m_Buffer->Unbind();

	m_DrawQueue.clear();
}

void RenderContext::EndFrame() {

	return;
}

}
