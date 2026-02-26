#include "origo/renderer/VaoCache.h"
#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/Helpers.h"
#include "origo/renderer/VertexLayoutRegistry.h"

namespace Origo {
const VaoCache::Entry& VaoCache::create_or_get(int layoutId, int heapId) {
	for (auto& e : s_entries)
		if (e.LayoutId == layoutId && e.HeapId == heapId)
			return e;

	return create(layoutId, heapId);
}

VaoCache::Entry& VaoCache::create(int layoutId, int heapId) {
	auto& e = s_entries.emplace_back();
	e.LayoutId = layoutId;
	e.HeapId = heapId;

	GeometryHeap* heap = GeometryHeapRegistry::get_heap(heapId);

	glGenVertexArrays(1, &e.VAO);
	glBindVertexArray(e.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, heap->get_vbo());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, heap->get_ibo());

	VertexLayout* layout = VertexLayoutRegistry::get(layoutId);
	auto& attribs { layout->get_attributes() };

	for (int i {}; i < attribs.size(); i++) {
		const auto& attrib { attribs[i] };
		const auto size { glsizeof(attrib.Type) * attrib.Amount };
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, attrib.Amount, attrib.Type, attrib.Normalized, layout->get_stride(), attrib.Offset));
	}

	return e;
}

}
