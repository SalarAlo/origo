#include "systems/EditorOutlineRenderSystem.h"

#include "origo/scene/SystemScheduler.h"
#include "origo/scene/GamePhase.h"

#include "origo/components/MeshRenderer.h"
#include "origo/components/Transform.h"
#include "origo/components/ModelRenderer.h"

#include "components/EditorOutline.h"

namespace OrigoEditor {
using namespace Origo;

void EditorOutlineRenderSystem::Render(Scene* scene, RenderContext& context) {
	scene->View<EditorOutline, TransformComponent, MeshRendererComponent>(
	    [&](RID entity,
	        EditorOutline& selection,
	        TransformComponent& transform,
	        MeshRendererComponent& mr) {
		    if (!selection.ShouldOutline || !mr.GetMesh().has_value())
			    return;

		    context.Submit(
		        *mr.GetMesh(),
		        EditorOutline::GetOutlineMaterial(),
		        transform.GetModelMatrix(),
		        RenderPass::Outline);
	    });

	scene->View<EditorOutline, TransformComponent, ModelRendererComponent>(
	    [&](RID entity,
	        EditorOutline& selection,
	        TransformComponent& transform,
	        ModelRendererComponent& mr) {
		    if (!selection.ShouldOutline || !mr.GetModel().has_value())
			    return;
		    // TODO: Make models outline work
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderEditor,
    OrigoEditor::EditorOutlineRenderSystem)
