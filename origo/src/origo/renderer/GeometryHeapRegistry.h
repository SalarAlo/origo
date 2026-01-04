#pragma once

#include "origo/renderer/GeometryHeap.h"
namespace Origo {

class GeometryHeapRegistry {
public:
	static int CreateHeap(int layoutId, GLenum usage, size_t vBytes, size_t iBytes) {
		auto& heaps = GetHeaps();
		heaps.emplace_back(std::make_unique<GeometryHeap>(layoutId, usage, vBytes, iBytes));
		return static_cast<int>(heaps.size() - 1);
	}

	static GeometryHeap* GetHeap(int heapId) {
		auto& heaps = GetHeaps();
		if (heapId < 0 || heapId >= static_cast<int>(heaps.size()))
			return nullptr;
		return heaps[heapId].get();
	}

private:
	static std::vector<std::unique_ptr<GeometryHeap>>& GetHeaps() {
		static std::vector<std::unique_ptr<GeometryHeap>> s_Heaps;
		return s_Heaps;
	}
};

}
