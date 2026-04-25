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
	static const Entry& create_or_get(int layoutId, int heapId);

private:
	inline static std::vector<Entry> s_entries {};

	static Entry& create(int layout, int heap);
};

}
