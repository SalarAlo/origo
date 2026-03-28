#include "origo/components/systems/TerrainMeshGeneratorSystem.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/Mesh.h"

#include "origo/components/TerrainComponent.h"

#include "origo/components/terrain/TerrainGenerator.h"

#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/VertexLayoutRegistry.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void TerrainMeshGeneratorSystem::update(Scene* scene, float dt) {
	(void)dt;

	scene->view<TerrainComponent>([](RID, TerrainComponent& terrain_component) {
		if (!terrain_component.RebuildNecessary)
			return;

		TerrainGenerator generator { terrain_component };
		const OptionalAssetHandle new_mesh = generator.get_mesh();
		if (!new_mesh.has_value())
			return;

		if (terrain_component.TerrainMesh.has_value()) {
			auto* old_mesh = AssetManager::get_instance().get_asset<Mesh>(*terrain_component.TerrainMesh);
			if (old_mesh) {
				size_t vertex_stride = 0;
				if (auto* layout = VertexLayoutRegistry::get(old_mesh->LayoutId))
					vertex_stride = layout->get_stride();

				if (vertex_stride > 0)
					GeometryHeapRegistry::free_mesh_range(old_mesh->HeapId, old_mesh->Range, vertex_stride);

				AssetManager::get_instance().destroy(*terrain_component.TerrainMesh);
			}
		}

		terrain_component.TerrainMesh = new_mesh;
		terrain_component.RebuildNecessary = false;
	});
}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateAlways,
    Origo::TerrainMeshGeneratorSystem);

}
