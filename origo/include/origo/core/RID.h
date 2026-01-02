#pragma once

namespace Origo {

class RID {
public:
	RID()
	    : m_ID(-1) { }

	RID(const RID& other) {
		m_ID = other.m_ID;
	}

	RID& operator=(const RID& other) {
		m_ID = other.m_ID;
		return *this;
	}

	static RID New() {
		static int current = 0;
		return RID(current++);
	}

	int GetId() const { return m_ID; }
	std::string ToString() const { return std::to_string(m_ID); }

	bool operator==(const RID& other) const noexcept {
		return m_ID == other.m_ID;
	}

private:
	explicit RID(int id)
	    : m_ID(id) { }

private:
	int m_ID;
};

}

// Hash
namespace std {
template <>
struct hash<Origo::RID> {
	size_t operator()(const Origo::RID& id) const noexcept {
		return std::hash<int>()(id.GetId());
	}
};
}
