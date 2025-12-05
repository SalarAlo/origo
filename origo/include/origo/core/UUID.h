#include <cstdint>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>

namespace Origo {

struct UUID {
	uint64_t High;
	uint64_t Low;

	UUID() {
		static std::random_device rd;
		static std::mt19937_64 gen(rd());
		High = gen();
		Low = gen();
	}

	UUID(uint64_t high_, uint64_t low_)
	    : High(high_)
	    , Low(low_) { }

	bool operator==(const UUID& other) const noexcept {
		return High == other.High && Low == other.Low;
	}

	std::string ToString() const {
		std::ostringstream ss;
		ss << std::hex << std::setfill('0')
		   << std::setw(16) << High << std::setw(16) << Low;
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
};
}

namespace std {
template <>
struct hash<Origo::UUID> {
	size_t operator()(const Origo::UUID& id) const noexcept {
		return std::hash<uint64_t>()(id.High ^ (id.Low << 1));
	}
};
}
