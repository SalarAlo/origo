#pragma once

namespace Origo {

class RID {
public:
	RID()
	    : m_id(-1) { }

	RID(const RID& other) {
		m_id = other.m_id;
	}

	RID& operator=(const RID& other) {
		m_id = other.m_id;
		return *this;
	}

	static RID new_rid() {
		return RID(next_id()++);
	}

	static RID from_id(int id) {
		if (id >= 0 && next_id() <= id)
			next_id() = id + 1;
		return RID(id);
	}

	static RID from_string(const std::string& str) {
		return from_id(std::stoi(str));
	}

	int get_id() const { return m_id; }
	std::string to_string() const { return std::to_string(m_id); }

	bool operator==(const RID& other) const noexcept {
		return m_id == other.m_id;
	}

private:
	static int& next_id() {
		static int current = 0;
		return current;
	}

	explicit RID(int id)
	    : m_id(id) { }

private:
	int m_id;
};

}

// Hash
namespace std {
template <>
struct hash<Origo::RID> {
	size_t operator()(const Origo::RID& id) const noexcept {
		return std::hash<int>()(id.get_id());
	}
};
}
