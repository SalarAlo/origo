#include "origo/components/systems/TerrainRenderSystem.h"

#include "origo/assets/DefaultAssetCache.h"

#include "origo/components/TerrainComponent.h"
#include "origo/components/Transform.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void TerrainRenderSystem::render(Scene* scene, RenderContext& context) {
	const AssetHandle material = DefaultAssetCache::get_instance().get_terrain_material();

	scene->view<TerrainComponent, TransformComponent>(
	    [&](RID,
	        TerrainComponent& terrain,
	        TransformComponent& transform) {
		    if (!terrain.TerrainMesh.has_value())
			    return;

		    context.submit_mesh(
		        *terrain.TerrainMesh,
		        material,
		        transform.get_model_matrix(),
		        RenderPass::Geometry);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::TerrainRenderSystem)
