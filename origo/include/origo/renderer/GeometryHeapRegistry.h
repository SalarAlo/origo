#pragma once

#include "origo/renderer/GeometryHeap.h"
namespace Origo {

class GeometryHeapRegistry {
public:
	static int CreateHeap(int layoutId, GLenum usage, size_t vBytes, size_t iBytes) {
		int id = m_Heaps.size();
		m_Heaps.emplace_back(layoutId, usage, vBytes, iBytes);
		return id;
	}

	static GeometryHeap* GetHeap(int id) {
		return &m_Heaps[id];
	}

private:
	inline static std::vector<GeometryHeap> m_Heaps {};
};

}
