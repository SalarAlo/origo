#include "TerrainComponentRenderSystem.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"

#include "origo/components/TerrainComponent.h"
#include "origo/components/TransformComponent.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void TerrainComponentRenderSystem::render(Scene* scene, RenderContext& context) {
	scene->view<TerrainComponent, TransformComponent>(
	    [&](RID entity,
	        TerrainComponent& terrain_component,
	        TransformComponent& transform) {
		    if (terrain_component.should_rebuild || terrain_component.started_rebuilding)
			    return;

		    if (!terrain_component.terrain_mesh.has_value())
			    return;

		    if (!AssetManager::get_instance().is_valid(*terrain_component.terrain_mesh))
			    return;

		    context.submit_mesh(
		        *terrain_component.terrain_mesh,
		        DefaultAssetCache::get_instance().get_color_material(),
		        transform.get_model_matrix(),
		        RenderPass::Geometry,
		        GL_TRIANGLES,
		        entity.get_id());
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::TerrainComponentRenderSystem)
