#pragma once

#include <cstdint>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>

namespace Origo {

struct UUID {
	UUID(bool isBadUuid = false)
	    : m_IsBadUuid(isBadUuid) {
		static std::random_device rd;
		static std::mt19937_64 gen(rd());
		if (!isBadUuid) {
			m_High = gen();
			m_Low = gen();
		} else {
			m_High = 0;
			m_Low = 0;
		}
	}

	UUID(uint64_t high, uint64_t low)
	    : m_High(high)
	    , m_Low(low)
	    , m_IsBadUuid(false) { }

	bool operator==(const UUID& other) const noexcept {
		return m_High == other.m_High && m_Low == other.m_Low;
	}

	std::string ToString() const {
		std::ostringstream ss;
		ss << std::hex << std::setfill('0')
		   << std::setw(16) << m_High << std::setw(16) << m_Low;
		return ss.str();
	}

	static UUID FromString(const std::string& str) {
		if (str.size() != 32) {
			throw std::invalid_argument("UUID length must be exactly 32 hex characters: " + str);
		}
		for (char c : str) {
			if (!std::isxdigit(static_cast<unsigned char>(c))) {
				throw std::invalid_argument("UUID contains non-hex characters");
			}
		}

		uint64_t high = std::stoull(str.substr(0, 16), nullptr, 16);
		uint64_t low = std::stoull(str.substr(16, 16), nullptr, 16);
		return UUID(high, low);
	}

	uint64_t GetHigh() const { return m_High; }
	uint64_t GetLow() const { return m_Low; }
	bool IsBad() { return m_IsBadUuid; }

private:
	uint64_t m_High;
	uint64_t m_Low;
	bool m_IsBadUuid;
};
}

namespace std {
template <>
struct hash<Origo::UUID> {
	size_t operator()(const Origo::UUID& id) const noexcept {
		return std::hash<uint64_t>()(id.GetHigh() ^ (id.GetLow() << 1));
	}
};
}
