#pragma once

#include <filesystem>
#include <stack>

#include "nlohmann/json.hpp"

#include "origo/serialization/ISerializer.h"

namespace Origo {
struct JsonStackEntry {
	nlohmann::json* Json = nullptr;
	bool IsArrayElement { false };
	size_t Index {};
};

class JsonSerializer : public ISerializer {
public:
	JsonSerializer(const std::filesystem::path& name);

	SERIALIZER_FWD
private:
	nlohmann::json& top_json();

	nlohmann::json m_root;
	std::stack<JsonStackEntry> m_objects_stack {};
};
}
