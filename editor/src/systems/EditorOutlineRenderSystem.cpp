#include "systems/EditorOutlineRenderSystem.h"

#include "components/EditorOutline.h"

#include "origo/assets/DefaultAssetCache.h"

#include "origo/components/MeshRenderer.h"
#include "origo/components/ModelRenderer.h"
#include "origo/components/Transform.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace OrigoEditor {
using namespace Origo;

void EditorOutlineRenderSystem::render(Scene* scene, RenderContext& context) {
	scene->view<EditorOutlineComponent, TransformComponent, MeshRendererComponent>(
	    [&](RID entity,
	        EditorOutlineComponent& selection,
	        TransformComponent& transform,
	        MeshRendererComponent& mr) {
		    if (!selection.ShouldOutline || !mr.MeshHandle.has_value())
			    return;

		    context.submit_mesh(
		        *mr.MeshHandle,
		        DefaultAssetCache::get_instance().get_outline_material(),
		        transform.get_model_matrix(),
		        RenderPass::Outline);
	    });

	scene->view<EditorOutlineComponent, TransformComponent, ModelRendererComponent>(
	    [&](RID entity,
	        EditorOutlineComponent& selection,
	        TransformComponent& transform,
	        ModelRendererComponent& mr) {
		    // FIXME: This right here does not work because stencil buffer are reset
		    // for each mesh render iteration, thus the models of the mesh get rendered
		    // seperately
		    //
		    // if (selection.ShouldOutline && mr.GetModel().has_value())
		    //  context.SubmitModel(*mr.GetModel(), transform.GetModelMatrix(), RenderPass::Outline, EditorOutlineComponent::GetOutlineMaterial());
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderEditor,
    OrigoEditor::EditorOutlineRenderSystem)
