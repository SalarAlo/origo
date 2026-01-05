#pragma once

#include "origo/renderer/GeometryHeap.h"

namespace Origo {

class GeometryHeapRegistry {
public:
	static int CreateHeap(
	    int layoutId,
	    GLenum usage,
	    size_t vertexBytes,
	    size_t indexBytes);

	static GeometryHeap* GetHeap(int heapId);

	static int GetOrCreateStaticMeshHeap(int layoutId);
	static int GetOrCreateDynamicMeshHeap(int layoutId);

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
	static std::vector<std::unique_ptr<GeometryHeap>>& GetHeaps();
	static std::unordered_map<HeapKey, int, HeapKeyHash>& GetHeapMap();
};

}
