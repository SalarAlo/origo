#include "systems/EditorOutlineRenderSystem.h"
#include "origo/scene/SystemScheduler.h"
#include "origo/scene/GamePhase.h"

#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"
#include "components/EditorSelection.h"

namespace OrigoEditor {

void EditorOutlineRenderSystem::Render(
    const Origo::Scene& scene,
    Origo::RenderContext& context) {
	auto selections = scene.GetAllComponentsOfType<EditorSelection>();

	for (auto* selection : selections) {
		if (!selection->IsSelected)
			continue;

		auto entityId = selection->AttachedTo->GetId();

		auto* transform = scene.GetComponent<Origo::Transform>(entityId);
		auto* mr = scene.GetComponent<Origo::MeshRenderer>(entityId);

		if (!transform || !mr)
			continue;

		context.Submit(
		    mr->GetMesh(),
		    EditorSelection::GetOutlineMaterial(),
		    transform,
		    Origo::RenderPass::Outline);
	}
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderEditor,
    OrigoEditor::EditorOutlineRenderSystem)
