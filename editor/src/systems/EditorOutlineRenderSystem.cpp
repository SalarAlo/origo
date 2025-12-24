#include "systems/EditorOutlineRenderSystem.h"
#include "origo/scene/SystemScheduler.h"
#include "origo/scene/GamePhase.h"

#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"
#include "components/EditorSelection.h"

namespace OrigoEditor {
using namespace Origo;

void EditorOutlineRenderSystem::Render(Scene* scene, RenderContext& context) {
	scene->View<EditorSelection, Transform, MeshRenderer>(
	    [&](RID entity,
	        EditorSelection& selection,
	        Transform& transform,
	        MeshRenderer& mr) {
		    if (!selection.IsSelected)
			    return;

		    context.Submit(
		        mr.GetMesh(),
		        EditorSelection::GetOutlineMaterial(),
		        &transform,
		        RenderPass::Outline);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderEditor,
    OrigoEditor::EditorOutlineRenderSystem)
