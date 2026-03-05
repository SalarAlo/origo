#include <filesystem>
#include <string_view>

#include "origo/serialization/JsonSerializer.h"

#include "origo/core/Logger.h"

#define JSON_DEF_WRITE_SERIALIZATION_FN(TYPE)                                                  \
	void JsonSerializer::write(std::string_view key, TYPE value) {                         \
		nlohmann::json& j = top_json();                                                \
		if (!j.is_object()) {                                                          \
			ORG_ERROR("Write('{}') on non-object (type: {})", key, j.type_name()); \
			return;                                                                \
		}                                                                              \
		j[std::string(key)] = value;                                                   \
	}

#define JSON_DEF_WRITE_SERIALIZATION_FN_ARRAY(TYPE)                                       \
	void JsonSerializer::write(TYPE value) {                                          \
		nlohmann::json& j = top_json();                                           \
		if (!j.is_array()) {                                                      \
			ORG_ERROR("Write(value) on non-array (type: {})", j.type_name()); \
			return;                                                           \
		}                                                                         \
		j.push_back(value);                                                       \
	}

#define JSON_DEF_READ_SERIALIZATION_FN(TYPE)                               \
	bool JsonSerializer::try_read(std::string_view key, TYPE& value) { \
		nlohmann::json& j = top_json();                            \
		if (!j.contains(std::string(key)))                         \
			return false;                                      \
		try {                                                      \
			value = j[std::string(key)].get<TYPE>();           \
			return true;                                       \
		} catch (...) {                                            \
			return false;                                      \
		}                                                          \
	}

namespace Origo {

template <typename T>
static bool try_read_array_object_impl(std::stack<JsonStackEntry>& stack, T& value) {
	if (stack.empty())
		return false;

	JsonStackEntry& entry = stack.top();
	nlohmann::json* j = entry.Json;

	if (!j || !j->is_array())
		return false;

	if (entry.Index >= j->size())
		return false;

	try {
		value = (*j)[entry.Index].get<T>();
		++entry.Index;
		return true;
	} catch (...) {
		return false;
	}
}

JsonSerializer::JsonSerializer(const std::filesystem::path& path)
    : ISerializer(path) {
	m_root = nlohmann::json::object();
	while (!m_objects_stack.empty())
		m_objects_stack.pop();
	m_objects_stack.push({ &m_root });
}

nlohmann::json& JsonSerializer::top_json() {
	auto* p = m_objects_stack.top().Json;
	if (!p) {
		ORG_ERROR("Json stack top was null; reinitializing to root");
		m_root = nlohmann::json::object();
		while (!m_objects_stack.empty())
			m_objects_stack.pop();
		m_objects_stack.push({ &m_root });
		p = &m_root;
	}
	return *p;
}

void JsonSerializer::save_to_file() {
	if (m_objects_stack.size() > 1) {
		ORG_ERROR("Trying to write while there are unclosed objects: {}", m_path.c_str());
		return;
	}

	std::filesystem::create_directories(m_path.parent_path());
	std::ofstream out(m_path, std::ios::out | std::ios::trunc);
	if (!out) {
		ORG_ERROR("Failed to open output file: {}", m_path.c_str());
		return;
	}
	out << top_json().dump(8);
}

void JsonSerializer::load_file() {
	std::ifstream in(m_path);
	if (!in) {
		ORG_ERROR("Failed to open input file: {}", m_path.c_str());
		m_root = nlohmann::json::object();
		while (!m_objects_stack.empty())
			m_objects_stack.pop();
		m_objects_stack.push({ &m_root });
		return;
	}

	try {
		nlohmann::json loaded;
		in >> loaded;
		m_root = loaded.is_object() ? std::move(loaded) : nlohmann::json::object();
	} catch (const std::exception& e) {
		ORG_ERROR("JSON parse error in {}: {}", m_path.c_str(), e.what());
		m_root = nlohmann::json::object();
	}

	while (!m_objects_stack.empty())
		m_objects_stack.pop();
	m_objects_stack.push({ &m_root });
}

void JsonSerializer::begin_object(std::string_view key) {
	nlohmann::json& parent = top_json();
	if (!parent.is_object()) {
		ORG_ERROR("BeginObject('{}') called on non-object (type: {})", key, parent.type_name());
		return;
	}

	nlohmann::json& new_obj = parent[std::string(key)];
	if (!new_obj.is_object())
		new_obj = nlohmann::json::object();

	m_objects_stack.push({ &new_obj });
}

void JsonSerializer::begin_array(std::string_view key) {
	nlohmann::json& parent = top_json();
	if (!parent.is_object()) {
		ORG_ERROR("BeginArray('{}') called on non-object (type: {})",
		    key, parent.type_name());
		return;
	}

	nlohmann::json& new_arr = parent[std::string(key)];
	if (!new_arr.is_array())
		new_arr = nlohmann::json::array();

	m_objects_stack.push({ &new_arr, false, 0 });
}

void JsonSerializer::begin_array_object() {
	nlohmann::json& arr = top_json();
	if (!arr.is_array()) {
		ORG_ERROR("BeginArrayobject called on non-array (type: {})", arr.type_name());
		return;
	}

	arr.push_back(nlohmann::json::object());
	size_t index = arr.size() - 1;
	m_objects_stack.push({ &arr[index], true, index });
}

void JsonSerializer::end_object() {
	if (m_objects_stack.size() > 1) {
		m_objects_stack.pop();
	} else {
		ORG_ERROR("Unbalanced EndObject. Resetting to root.");
		while (!m_objects_stack.empty())
			m_objects_stack.pop();
		m_objects_stack.push({ &m_root });
	}
}

void JsonSerializer::end_array() {
	if (m_objects_stack.size() > 1) {
		m_objects_stack.pop();
	} else {
		ORG_ERROR("Unbalanced EndArray. Resetting to root.");
		while (!m_objects_stack.empty())
			m_objects_stack.pop();
		m_objects_stack.push({ &m_root });
	}
}

void JsonSerializer::end_array_object() {
	if (m_objects_stack.size() > 1) {
		m_objects_stack.pop();
	} else {
		ORG_ERROR("Unbalanced EndArrayobject. Resetting to root.");
		while (!m_objects_stack.empty())
			m_objects_stack.pop();
		m_objects_stack.push({ &m_root });
	}
}

bool JsonSerializer::try_read_array_object(int& value) {
	return try_read_array_object_impl(m_objects_stack, value);
}

bool JsonSerializer::try_read_array_object(unsigned int& value) {
	return try_read_array_object_impl(m_objects_stack, value);
}

bool JsonSerializer::try_read_array_object(float& value) {
	return try_read_array_object_impl(m_objects_stack, value);
}

bool JsonSerializer::try_read_array_object(std::string& value) {
	return try_read_array_object_impl(m_objects_stack, value);
}

bool JsonSerializer::try_begin_array_object_read() {
	if (m_objects_stack.empty())
		return false;

	JsonStackEntry& entry = m_objects_stack.top();
	nlohmann::json* j = entry.Json;

	if (!j || !j->is_array())
		return false;

	if (entry.Index >= j->size())
		return false;

	nlohmann::json& elem = (*j)[entry.Index++];
	m_objects_stack.push({ &elem, true, 0 });
	return true;
}

JSON_DEF_WRITE_SERIALIZATION_FN(std::string_view)
JSON_DEF_WRITE_SERIALIZATION_FN(int)
JSON_DEF_WRITE_SERIALIZATION_FN(unsigned int)
JSON_DEF_WRITE_SERIALIZATION_FN(float)

JSON_DEF_READ_SERIALIZATION_FN(std::string)
JSON_DEF_READ_SERIALIZATION_FN(int)
JSON_DEF_READ_SERIALIZATION_FN(unsigned int)
JSON_DEF_READ_SERIALIZATION_FN(float)

JSON_DEF_WRITE_SERIALIZATION_FN_ARRAY(std::string_view)
JSON_DEF_WRITE_SERIALIZATION_FN_ARRAY(int)
JSON_DEF_WRITE_SERIALIZATION_FN_ARRAY(unsigned int)
JSON_DEF_WRITE_SERIALIZATION_FN_ARRAY(float)

}
