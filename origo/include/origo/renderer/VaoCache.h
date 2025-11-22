#pragma once

namespace Origo {

class VaoCache {
public:
	struct Entry {
		int LayoutId;
		int HeapId;
		GLuint VAO;
	};

public:
	VaoCache() = default;
	~VaoCache();
	const Entry& CreateOrGet(int layoutId, int heapId);

private:
	std::vector<Entry> m_Entries;

	Entry& Create(int layout, int heap);
};

}
