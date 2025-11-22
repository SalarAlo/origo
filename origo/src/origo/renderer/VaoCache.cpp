#include "origo/renderer/VaoCache.h"
#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/Helpers.h"
#include "origo/renderer/VertexLayoutRegistry.h"

namespace Origo {
const VaoCache::Entry& VaoCache::CreateOrGet(int layoutId, int heapId) {
	for (auto& e : m_Entries)
		if (e.LayoutId == layoutId && e.HeapId == heapId)
			return e;

	return Create(layoutId, heapId);
}

VaoCache::Entry& VaoCache::Create(int layoutId, int heapId) {
	auto& e = m_Entries.emplace_back();
	e.LayoutId = layoutId;
	e.HeapId = heapId;

	GeometryHeap* heap = GeometryHeapRegistry::GetHeap(heapId);

	glGenVertexArrays(1, &e.VAO);
	glBindVertexArray(e.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, heap->GetVbo());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, heap->GetIbo());

	VertexLayout* layout = VertexLayoutRegistry::Get(layoutId);
	auto& attribs { layout->GetAttributes() };
	int stride {};

	for (int i {}; i < attribs.size(); i++) {
		auto& attrib { attribs[i] };
		const auto size { Glsizeof(attrib.Type) * attrib.Amount };
		stride += size;
	}

	long offset {};
	for (int i {}; i < attribs.size(); i++) {
		const auto& attrib { attribs[i] };
		const auto size { Glsizeof(attrib.Type) * attrib.Amount };
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, attrib.Amount, attrib.Type, attrib.Normalized, stride, reinterpret_cast<void*>(offset)));
		offset += size;
	}

	glBindVertexArray(0);

	return e;
}

}
