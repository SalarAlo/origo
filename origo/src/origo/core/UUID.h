#pragma once

#include <cstdint>
#include <random>
#include <sstream>
#include <iomanip>

namespace Origo {

struct UUID {
	static UUID generate() {
		static std::random_device rd;
		static std::mt19937_64 gen(rd());

		auto high = gen();
		auto low = gen();

		return UUID(low, high);
	}

	bool operator==(const UUID& other) const noexcept {
		return m_high == other.m_high && m_low == other.m_low;
	}

	std::string to_string() const {
		std::ostringstream ss;
		ss << std::hex << std::setfill('0')
		   << std::setw(16) << m_high << std::setw(16) << m_low;
		return ss.str();
	}

	static UUID from_string(const std::string& str) {
		if (str.size() != 32) {
			return UUID(0, 0);
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

	static UUID from_arbitrary_string(std::string_view str) {
		uint64_t high = hash_fn_v1a64(str);
		uint64_t low = hash_fn_v1a64("origo::uuid::salt::" + std::string(str));
		return UUID(high, low);
	}

	static uint64_t hash_fn_v1a64(std::string_view str) {
		constexpr uint64_t fnv_offset = 14695981039346656037ull;
		constexpr uint64_t fnv_prime = 1099511628211ull;

		uint64_t hash = fnv_offset;
		for (char c : str) {
			hash ^= static_cast<uint8_t>(c);
			hash *= fnv_prime;
		}
		return hash;
	}

	static UUID from_hash(std::string_view str) {
		uint64_t h1 = hash_fn_v1a64(str);
		uint64_t h2 = hash_fn_v1a64("origo::uuid::salt::" + std::string(str));

		return UUID(h1, h2);
	}

	uint64_t get_high() const { return m_high; }
	uint64_t get_low() const { return m_low; }

private:
	UUID() = default;
	UUID(uint64_t high, uint64_t low)
	    : m_high(high)
	    , m_low(low) { }

private:
	uint64_t m_high;
	uint64_t m_low;
};
}

using OptionalUUID = std::optional<Origo::UUID>;

namespace std {
template <>
struct hash<Origo::UUID> {
	size_t operator()(const Origo::UUID& id) const noexcept {
		return std::hash<uint64_t>()(id.get_high() ^ (id.get_low() << 1));
	}
};

}
