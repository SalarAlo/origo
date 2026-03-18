#include "origo/components/terrain/TerrainGenerator.h"

#include <algorithm>
#include <vector>

#include "origo/assets/AssetFactory.h"
#include "origo/assets/Mesh.h"

#include "origo/core/Random.h"

#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/VertexLayout.h"

namespace Origo {

OptionalAssetHandle TerrainGenerator::get_mesh() {
	if (m_terrain_component.Detail <= 0.0f || m_terrain_component.Size <= 0)
		return std::nullopt;

	auto perlin_settings { m_terrain_component.Perlin };
	const int width = static_cast<int>(m_terrain_component.Size / m_terrain_component.Detail) + 1;
	if (width < 2)
		return std::nullopt;

	std::vector<float> heights(width * width, 0.0f);
	for (int z = 0; z < width; ++z) {
		for (int x = 0; x < width; ++x) {
			const float world_x = x * m_terrain_component.Detail;
			const float world_z = z * m_terrain_component.Detail;
			heights[z * width + x] = Random::perlin_octaves(world_x, world_z, perlin_settings) * m_terrain_component.Height;
		}
	}

	std::vector<float> points {};
	std::vector<unsigned int> triangles {};
	points.reserve(static_cast<size_t>(width) * static_cast<size_t>(width) * 12);
	triangles.reserve(static_cast<size_t>(width - 1) * static_cast<size_t>(width - 1) * 6);

	for (int z = 0; z < width; ++z) {
		for (int x = 0; x < width; ++x) {
			const float world_x = x * m_terrain_component.Detail;
			const float world_z = z * m_terrain_component.Detail;
			const float height = heights[z * width + x];
			const int left_x = std::max(0, x - 1);
			const int right_x = std::min(width - 1, x + 1);
			const int down_z = std::max(0, z - 1);
			const int up_z = std::min(width - 1, z + 1);
			const float height_left = heights[z * width + left_x];
			const float height_right = heights[z * width + right_x];
			const float height_down = heights[down_z * width + x];
			const float height_up = heights[up_z * width + x];
			const Vec3 normal = glm::normalize(Vec3(
			    height_left - height_right,
			    2.0f * m_terrain_component.Detail,
			    height_down - height_up));
			const float u = static_cast<float>(x) / static_cast<float>(width - 1);
			const float v = static_cast<float>(z) / static_cast<float>(width - 1);

			// Layout: position(3), normal(3), texcoord(2), tangent(4)
			points.push_back(world_x);
			points.push_back(height);
			points.push_back(world_z);
			points.push_back(normal.x);
			points.push_back(normal.y);
			points.push_back(normal.z);
			points.push_back(u);
			points.push_back(v);
			points.push_back(1.0f);
			points.push_back(0.0f);
			points.push_back(0.0f);
			points.push_back(1.0f);
		}
	}

	for (int z = 0; z < width - 1; ++z) {
		for (int x = 0; x < width - 1; ++x) {
			int top_left = z * width + x;
			int top_right = top_left + 1;
			int bottom_left = (z + 1) * width + x;
			int bottom_right = bottom_left + 1;

			triangles.push_back(top_left);
			triangles.push_back(bottom_left);
			triangles.push_back(top_right);

			triangles.push_back(top_right);
			triangles.push_back(bottom_left);
			triangles.push_back(bottom_right);
		}
	}

	const int layout_id = VertexLayout::get_static_mesh_tangent_layout();
	const int heap_id = GeometryHeapRegistry::get_or_create_static_mesh_heap(layout_id);
	GeometryHeap* heap = GeometryHeapRegistry::get_heap(heap_id);
	if (!heap)
		return std::nullopt;

	auto range = heap->allocate(
	    points.data(),
	    points.size() * sizeof(float),
	    12 * sizeof(float),
	    triangles.data(),
	    triangles.size());

	return AssetFactory::get_instance().create_runtime_asset<Mesh>(
	    "TerrainMesh",
	    layout_id,
	    heap_id,
	    range);
}
}
