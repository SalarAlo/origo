#include "GeometryHeapRegistry.h"

namespace Origo {

static constexpr size_t DefaultStaticVertexBytes = 8 * 1024 * 1024; // 8 MB
static constexpr size_t DefaultStaticIndexBytes = 4 * 1024 * 1024; // 4 MB

static constexpr size_t DefaultDynamicVertexBytes = 2 * 1024 * 1024; // 2 MB
static constexpr size_t DefaultDynamicIndexBytes = 1 * 1024 * 1024; // 1 MB

int GeometryHeapRegistry::CreateHeap(
    int layoutId,
    GLenum usage,
    size_t vertexBytes,
    size_t indexBytes) {
	auto& heaps = GetHeaps();
	heaps.emplace_back(
	    std::make_unique<GeometryHeap>(layoutId, usage, vertexBytes, indexBytes));
	return static_cast<int>(heaps.size() - 1);
}

GeometryHeap* GeometryHeapRegistry::GetHeap(int heapId) {
	auto& heaps = GetHeaps();
	if (heapId < 0 || heapId >= static_cast<int>(heaps.size()))
		return nullptr;
	return heaps[heapId].get();
}

int GeometryHeapRegistry::GetOrCreateStaticMeshHeap(int layoutId) {
	HeapKey key { layoutId, GL_STATIC_DRAW };
	auto& map = GetHeapMap();

	auto it = map.find(key);
	if (it != map.end())
		return it->second;

	int heapId = CreateHeap(
	    layoutId,
	    GL_STATIC_DRAW,
	    DefaultStaticVertexBytes,
	    DefaultStaticIndexBytes);

	map[key] = heapId;
	return heapId;
}

int GeometryHeapRegistry::GetOrCreateDynamicMeshHeap(int layoutId) {
	HeapKey key { layoutId, GL_DYNAMIC_DRAW };
	auto& map = GetHeapMap();

	auto it = map.find(key);
	if (it != map.end())
		return it->second;

	int heapId = CreateHeap(
	    layoutId,
	    GL_DYNAMIC_DRAW,
	    DefaultDynamicVertexBytes,
	    DefaultDynamicIndexBytes);

	map[key] = heapId;
	return heapId;
}

std::vector<std::unique_ptr<GeometryHeap>>&
GeometryHeapRegistry::GetHeaps() {
	static std::vector<std::unique_ptr<GeometryHeap>> s_Heaps;
	return s_Heaps;
}

std::unordered_map<
    GeometryHeapRegistry::HeapKey,
    int,
    GeometryHeapRegistry::HeapKeyHash>&
GeometryHeapRegistry::GetHeapMap() {
	static std::unordered_map<HeapKey, int, HeapKeyHash> s_HeapMap;
	return s_HeapMap;
}

}
