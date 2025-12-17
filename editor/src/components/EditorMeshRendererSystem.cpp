#include "origo/renderer/RenderContext.h"
#include "origo/renderer/RenderableRegistry.h"
#include "origo/scene/Scene.h"
#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"
#include "components/EditorMeshRenderer.h"

namespace OrigoEditor {
static void RenderEditorMeshRenderers(const Origo::Scene& scene, Origo::RenderContext& context) {
	auto meshRenderers = scene.GetAllComponentsOfType<OrigoEditor::EditorMeshRenderer>();

	for (auto* emr : meshRenderers) {
		auto* transform = scene.GetComponent<Origo::Transform>(emr->AttachedTo->GetId());

		context.Submit(
		    emr->GetMesh(),
		    emr->GetMaterial(),
		    transform,
		    Origo::RenderPass::Geometry);

		if (emr->IsSelected) {
			context.Submit(
			    emr->GetMesh(),
			    emr->GetMaterial(),
			    transform,
			    Origo::RenderPass::SelectedMask);

			context.Submit(
			    emr->GetMesh(),
			    EditorMeshRenderer::GetOutlineMaterial(),
			    transform,
			    Origo::RenderPass::Outline);
		}
	}
}

REGISTER_RENDERABLE(RenderEditorMeshRenderers)
}
