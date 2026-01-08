#include "systems/EditorOutlineRenderSystem.h"
#include "origo/scene/ModelRenderer.h"
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
		    if (!selection.ShouldOutline || !mr.GetMesh().has_value())
			    return;

		    context.Submit(
		        *mr.GetMesh(),
		        EditorOutline::GetOutlineMaterial(),
		        transform.GetModelMatrix(),
		        RenderPass::Outline);
	    });

	scene->View<EditorOutline, Transform, ModelRenderer>(
	    [&](RID entity,
	        EditorOutline& selection,
	        Transform& transform,
	        ModelRenderer& mr) {
		    if (!selection.ShouldOutline || !mr.GetModel().has_value())
			    return;
		    // TODO: Make models outline work
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderEditor,
    OrigoEditor::EditorOutlineRenderSystem)
