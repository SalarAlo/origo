#include "GeometryHeapRegistry.h"

namespace Origo {

static constexpr size_t default_static_vertex_bytes = 8 * 1024 * 1024; // 8 MB
static constexpr size_t default_static_index_bytes = 4 * 1024 * 1024; // 4 MB

static constexpr size_t default_dynamic_vertex_bytes = 2 * 1024 * 1024; // 2 MB
static constexpr size_t default_dynamic_index_bytes = 1 * 1024 * 1024; // 1 MB

int GeometryHeapRegistry::create_heap(
    int layoutId,
    GLenum usage,
    size_t vertexBytes,
    size_t indexBytes) {
	auto& heaps = get_heaps();
	heaps.emplace_back(
	    std::make_unique<GeometryHeap>(layoutId, usage, vertexBytes, indexBytes));
	return static_cast<int>(heaps.size() - 1);
}

GeometryHeap* GeometryHeapRegistry::get_heap(int heapId) {
	auto& heaps = get_heaps();
	if (heapId < 0 || heapId >= static_cast<int>(heaps.size()))
		return nullptr;
	return heaps[heapId].get();
}

int GeometryHeapRegistry::get_or_create_static_mesh_heap(int layoutId) {
	HeapKey key { layoutId, GL_STATIC_DRAW };
	auto& map = get_heap_map();

	auto it = map.find(key);
	if (it != map.end())
		return it->second;

	int heap_id = create_heap(
	    layoutId,
	    GL_STATIC_DRAW,
	    default_static_vertex_bytes,
	    default_static_index_bytes);

	map[key] = heap_id;
	return heap_id;
}

int GeometryHeapRegistry::get_or_create_dynamic_mesh_heap(int layoutId) {
	HeapKey key { layoutId, GL_DYNAMIC_DRAW };
	auto& map = get_heap_map();

	auto it = map.find(key);
	if (it != map.end())
		return it->second;

	int heap_id = create_heap(
	    layoutId,
	    GL_DYNAMIC_DRAW,
	    default_dynamic_vertex_bytes,
	    default_dynamic_index_bytes);

	map[key] = heap_id;
	return heap_id;
}

std::vector<std::unique_ptr<GeometryHeap>>&
GeometryHeapRegistry::get_heaps() {
	static std::vector<std::unique_ptr<GeometryHeap>> s_heaps;
	return s_heaps;
}

std::unordered_map<
    GeometryHeapRegistry::HeapKey,
    int,
    GeometryHeapRegistry::HeapKeyHash>&
GeometryHeapRegistry::get_heap_map() {
	static std::unordered_map<HeapKey, int, HeapKeyHash> s_heap_map;
	return s_heap_map;
}

}
