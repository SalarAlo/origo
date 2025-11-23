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
	static const Entry& CreateOrGet(int layoutId, int heapId);

private:
	inline static std::vector<Entry> s_Entries {};

	static Entry& Create(int layout, int heap);
};

}
