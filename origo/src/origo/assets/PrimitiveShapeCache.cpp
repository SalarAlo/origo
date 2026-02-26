#include "PrimitiveShapeCache.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Mesh.h"
#include "origo/assets/MeshData.h"
#include "origo/assets/PrimitiveShape.h"

#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/VertexLayout.h"
#include "origo/renderer/VertexLayoutRegistry.h"

namespace Origo {

void PrimitiveShapeCache::create_all_primitive_shapes() {
	get_cube_mesh();
	get_sphere_mesh();
	get_cone_mesh();
}

AssetHandle PrimitiveShapeCache::get_cube_mesh() {
	static auto cube_handle = ([] {
		auto vertex_layout_id { VertexLayout::get_static_mesh_layout() };
		auto heap_id { GeometryHeapRegistry::get_or_create_static_mesh_heap(vertex_layout_id) };
		auto heap = GeometryHeapRegistry::get_heap(heap_id);

		MeshData data = get_data_from_shape(PrimitiveShape::Cube);
		auto vertex_stride = VertexLayoutRegistry::get(vertex_layout_id)->get_stride();

		auto range = heap->allocate(
		    data.Vertices.data(),
		    data.Vertices.size() * sizeof(float),
		    vertex_stride,
		    data.Indices.data(),
		    data.Indices.size());

		UUID mesh_id { UUID::from_arbitrary_string("DEFAULT_CUBE_MESH") };
		auto cube_handle = AssetFactory::get_instance().create_synthetic_asset<Mesh>(
		    "Default Cube",
		    mesh_id,
		    vertex_layout_id,
		    heap_id,
		    range);

		return cube_handle;
	})();
	return cube_handle;
}

AssetHandle PrimitiveShapeCache::get_sphere_mesh() {
	static auto cube_handle = ([] {
		auto vertex_layout_id { VertexLayout::get_static_mesh_layout() };
		auto heap_id { GeometryHeapRegistry::get_or_create_static_mesh_heap(vertex_layout_id) };
		auto heap = GeometryHeapRegistry::get_heap(heap_id);

		MeshData data = get_data_from_shape(PrimitiveShape::Sphere);
		auto vertex_stride = VertexLayoutRegistry::get(vertex_layout_id)->get_stride();

		auto range = heap->allocate(
		    data.Vertices.data(),
		    data.Vertices.size() * sizeof(float),
		    vertex_stride,
		    data.Indices.data(),
		    data.Indices.size());

		UUID mesh_id { UUID::from_arbitrary_string("DEFAULT_SPHERE_MESH") };
		auto cube_handle = AssetFactory::get_instance().create_synthetic_asset<Mesh>(
		    "Default Sphere", mesh_id, vertex_layout_id,
		    heap_id,
		    range);

		return cube_handle;
	})();
	return cube_handle;
}

AssetHandle PrimitiveShapeCache::get_cone_mesh() {
	static auto cone_handle = ([] {
		auto vertex_layout_id { VertexLayout::get_static_mesh_layout() };
		auto heap_id { GeometryHeapRegistry::get_or_create_static_mesh_heap(vertex_layout_id) };
		auto heap = GeometryHeapRegistry::get_heap(heap_id);

		MeshData data = get_data_from_shape(PrimitiveShape::Cone);
		auto vertex_stride = VertexLayoutRegistry::get(vertex_layout_id)->get_stride();

		auto range = heap->allocate(
		    data.Vertices.data(),
		    data.Vertices.size() * sizeof(float),
		    vertex_stride,
		    data.Indices.data(),
		    data.Indices.size());

		UUID mesh_id { UUID::from_arbitrary_string("DEFAULT_CONE_MESH") };
		auto cone_handle = AssetFactory::get_instance().create_synthetic_asset<Mesh>(
		    "Default Cone",
		    mesh_id,
		    vertex_layout_id,
		    heap_id,
		    range);

		return cone_handle;
	})();
	return cone_handle;
}

}
