#pragma once

#include "nlohmann/json.hpp"
#include "origo/serialization/ISerializer.h"
#include <stack>

namespace Origo {
struct JsonStackEntry {
	nlohmann::json* Json = nullptr;
	bool IsArrayElement { false };
	size_t Index {};
};

class JsonSerializer : public ISerializer {
public:
	JsonSerializer(std::string_view name);

	SERIALIZER_FWD
private:
	nlohmann::json& top_json();

	nlohmann::json m_Root;
	std::stack<JsonStackEntry> m_ObjectsStack {};
};
}
