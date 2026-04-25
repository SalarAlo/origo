#pragma once

#include "origo/renderer/GeometryHeap.h"

namespace Origo {

class GeometryHeapRegistry {
public:
	static int create_heap(
	    int layoutId,
	    GLenum usage,
	    size_t vertexBytes,
	    size_t indexBytes);

	static GeometryHeap* get_heap(int heapId);
	static bool free_mesh_range(int heapId, const MeshRange& range, size_t vertexStrideBytes);

	static int get_or_create_static_mesh_heap(int layoutId);
	static int get_or_create_dynamic_mesh_heap(int layoutId);

private:
	struct HeapKey {
		int LayoutId;
		GLenum Usage;

		bool operator==(const HeapKey& other) const {
			return LayoutId == other.LayoutId && Usage == other.Usage;
		}
	};

	struct HeapKeyHash {
		size_t operator()(const HeapKey& k) const {
			return std::hash<int>()(k.LayoutId) ^ (std::hash<int>()(k.Usage) << 1);
		}
	};

private:
	static std::vector<std::unique_ptr<GeometryHeap>>& get_heaps();
	static std::unordered_map<HeapKey, int, HeapKeyHash>& get_heap_map();
};

}
