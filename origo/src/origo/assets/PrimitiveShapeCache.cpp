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

void PrimitiveShapeCache::CreateAllPrimitiveShapes() {
	GetCubeMesh();
	GetSphereMesh();
	GetConeMesh();
}

AssetHandle PrimitiveShapeCache::GetCubeMesh() {
	static auto cubeHandle = ([] {
		auto vertexLayoutID { VertexLayout::GetStaticMeshLayout() };
		auto heapID { GeometryHeapRegistry::GetOrCreateStaticMeshHeap(vertexLayoutID) };
		auto heap = GeometryHeapRegistry::GetHeap(heapID);

		MeshData data = GetDataFromShape(PrimitiveShape::Cube);
		auto vertexStride = VertexLayoutRegistry::Get(vertexLayoutID)->GetStride();

		auto range = heap->Allocate(
		    data.Vertices.data(),
		    data.Vertices.size() * sizeof(float),
		    vertexStride,
		    data.Indices.data(),
		    data.Indices.size());

		UUID meshID { UUID::FromArbitraryString("DEFAULT_CUBE_MESH") };
		auto cubeHandle = AssetFactory::GetInstance().CreateSyntheticAsset<Mesh>(
		    "Default Cube",
		    meshID,
		    vertexLayoutID,
		    heapID,
		    range);

		return cubeHandle;
	})();
	return cubeHandle;
}

AssetHandle PrimitiveShapeCache::GetSphereMesh() {
	static auto cubeHandle = ([] {
		auto vertexLayoutID { VertexLayout::GetStaticMeshLayout() };
		auto heapID { GeometryHeapRegistry::GetOrCreateStaticMeshHeap(vertexLayoutID) };
		auto heap = GeometryHeapRegistry::GetHeap(heapID);

		MeshData data = GetDataFromShape(PrimitiveShape::Sphere);
		auto vertexStride = VertexLayoutRegistry::Get(vertexLayoutID)->GetStride();

		auto range = heap->Allocate(
		    data.Vertices.data(),
		    data.Vertices.size() * sizeof(float),
		    vertexStride,
		    data.Indices.data(),
		    data.Indices.size());

		UUID meshID { UUID::FromArbitraryString("DEFAULT_SPHERE_MESH") };
		auto cubeHandle = AssetFactory::GetInstance().CreateSyntheticAsset<Mesh>(
		    "Default Sphere", meshID, vertexLayoutID,
		    heapID,
		    range);

		return cubeHandle;
	})();
	return cubeHandle;
}

AssetHandle PrimitiveShapeCache::GetConeMesh() {
	static auto coneHandle = ([] {
		auto vertexLayoutID { VertexLayout::GetStaticMeshLayout() };
		auto heapID { GeometryHeapRegistry::GetOrCreateStaticMeshHeap(vertexLayoutID) };
		auto heap = GeometryHeapRegistry::GetHeap(heapID);

		MeshData data = GetDataFromShape(PrimitiveShape::Cone);
		auto vertexStride = VertexLayoutRegistry::Get(vertexLayoutID)->GetStride();

		auto range = heap->Allocate(
		    data.Vertices.data(),
		    data.Vertices.size() * sizeof(float),
		    vertexStride,
		    data.Indices.data(),
		    data.Indices.size());

		UUID meshID { UUID::FromArbitraryString("DEFAULT_CONE_MESH") };
		auto coneHandle = AssetFactory::GetInstance().CreateSyntheticAsset<Mesh>(
		    "Default Cone",
		    meshID,
		    vertexLayoutID,
		    heapID,
		    range);

		return coneHandle;
	})();
	return coneHandle;
}

}
