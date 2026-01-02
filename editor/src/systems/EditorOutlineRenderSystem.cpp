#include "systems/EditorOutlineRenderSystem.h"
#include "origo/scene/SystemScheduler.h"
#include "origo/scene/GamePhase.h"

#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"
#include "components/EditorOutline.h"

namespace OrigoEditor {
using namespace Origo;

void EditorOutlineRenderSystem::Render(Scene* scene, RenderContext& context) {
	scene->View<EditorOutline, Transform, MeshRenderer>(
	    [&](RID entity,
	        EditorOutline& selection,
	        Transform& transform,
	        MeshRenderer& mr) {
		    if (!selection.ShouldOutline)
			    return;

		    context.Submit(
		        mr.GetMesh(),
		        EditorOutline::GetOutlineMaterial(),
		        &transform,
		        RenderPass::Outline);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderEditor,
    OrigoEditor::EditorOutlineRenderSystem)
