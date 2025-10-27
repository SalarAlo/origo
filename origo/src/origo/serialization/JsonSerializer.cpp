#include "origo/serialization/JsonSerializer.h"
#include "origo/core/Logger.h"
#include <string_view>

#define JSON_DEF_WRITE_SERIALIZATION_FN(TYPE)                                                  \
	void JsonSerializer::Write(std::string_view key, TYPE value) {                         \
		nlohmann::json& j = top_json();                                                \
		if (!j.is_object()) {                                                          \
			ORG_ERROR("Write('{}') on non-object (type: {})", key, j.type_name()); \
			return;                                                                \
		}                                                                              \
		j[std::string(key)] = value;                                                   \
	}

#define JSON_DEF_WRITE_SERIALIZATION_FN_ARRAY(TYPE)                                       \
	void JsonSerializer::Write(TYPE value) {                                          \
		nlohmann::json& j = top_json();                                           \
		if (!j.is_array()) {                                                      \
			ORG_ERROR("Write(value) on non-array (type: {})", j.type_name()); \
			return;                                                           \
		}                                                                         \
		j.push_back(value);                                                       \
	}

#define JSON_DEF_READ_SERIALIZATION_FN(TYPE)                              \
	bool JsonSerializer::TryRead(std::string_view key, TYPE& value) { \
		nlohmann::json& j = top_json();                           \
		if (!j.contains(std::string(key)))                        \
			return false;                                     \
		try {                                                     \
			value = j[std::string(key)].get<TYPE>();          \
			return true;                                      \
		} catch (...) {                                           \
			return false;                                     \
		}                                                         \
	}

namespace Origo {

JsonSerializer::JsonSerializer(std::string_view name)
    : ISerializer(name) {
	m_Root = nlohmann::json::object();
	while (!m_ObjectsStack.empty())
		m_ObjectsStack.pop();
	m_ObjectsStack.push({ &m_Root });
}

nlohmann::json& JsonSerializer::top_json() {
	auto* p = m_ObjectsStack.top().Json;
	if (!p) {
		// Last-ditch safety: re-anchor to root
		ORG_ERROR("Json stack top was null; reinitializing to root");
		m_Root = nlohmann::json::object();
		while (!m_ObjectsStack.empty())
			m_ObjectsStack.pop();
		m_ObjectsStack.push({ &m_Root });
		p = &m_Root;
	}
	return *p;
}

void JsonSerializer::WriteFile() {
	if (m_ObjectsStack.size() > 1) {
		ORG_ERROR("Trying to write while there are unclosed objects: {}", m_Path);
		return;
	}
	std::ofstream out(m_Path, std::ios::out | std::ios::trunc);
	if (!out) {
		ORG_ERROR("Failed to open output file: {}", m_Path);
		return;
	}
	out << top_json().dump(8);
}

void JsonSerializer::LoadFile() {
	std::ifstream in(m_Path);
	if (!in) {
		ORG_ERROR("Failed to open input file: {}", m_Path);
		// Always leave a valid empty root
		m_Root = nlohmann::json::object();
		while (!m_ObjectsStack.empty())
			m_ObjectsStack.pop();
		m_ObjectsStack.push({ &m_Root });
		return;
	}

	try {
		nlohmann::json loaded;
		in >> loaded;
		m_Root = loaded.is_object() ? std::move(loaded) : nlohmann::json::object();
	} catch (const std::exception& e) {
		ORG_ERROR("JSON parse error in {}: {}", m_Path, e.what());
		m_Root = nlohmann::json::object();
	}

	while (!m_ObjectsStack.empty())
		m_ObjectsStack.pop();
	m_ObjectsStack.push({ &m_Root });
}

void JsonSerializer::BeginObject(std::string_view key) {
	nlohmann::json& parent = top_json();
	if (!parent.is_object()) {
		ORG_ERROR("BeginObject('{}') called on non-object (type: {})",
		    key, parent.type_name());
		return;
	}

	nlohmann::json& newObj = parent[std::string(key)];
	if (!newObj.is_object())
		newObj = nlohmann::json::object();

	m_ObjectsStack.push({ &newObj });
}

void JsonSerializer::BeginArray(std::string_view key) {
	nlohmann::json& parent = top_json();
	if (!parent.is_object()) {
		ORG_ERROR("BeginArray('{}') called on non-object (type: {})",
		    key, parent.type_name());
		return;
	}

	nlohmann::json& newArr = parent[std::string(key)];
	if (!newArr.is_array())
		newArr = nlohmann::json::array();

	m_ObjectsStack.push({ &newArr });
}

void JsonSerializer::BeginArrayElement() {
	nlohmann::json& arr = top_json();
	if (!arr.is_array()) {
		ORG_ERROR("BeginArrayElement called on non-array (type: {})", arr.type_name());
		return;
	}

	arr.push_back(nlohmann::json::object());
	size_t index = arr.size() - 1;
	m_ObjectsStack.push({ &arr[index], true, index });
}

void JsonSerializer::EndObject() {
	if (m_ObjectsStack.size() > 1) {
		m_ObjectsStack.pop();
	} else {
		ORG_ERROR("Unbalanced EndObject. Resetting to root.");
		while (!m_ObjectsStack.empty())
			m_ObjectsStack.pop();
		m_ObjectsStack.push({ &m_Root });
	}
}

void JsonSerializer::EndArray() {
	if (m_ObjectsStack.size() > 1) {
		m_ObjectsStack.pop();
	} else {
		ORG_ERROR("Unbalanced EndArray. Resetting to root.");
		while (!m_ObjectsStack.empty())
			m_ObjectsStack.pop();
		m_ObjectsStack.push({ &m_Root });
	}
}

void JsonSerializer::EndArrayElement() {
	if (m_ObjectsStack.size() > 1) {
		m_ObjectsStack.pop();
	} else {
		ORG_ERROR("Unbalanced EndArrayElement. Resetting to root.");
		while (!m_ObjectsStack.empty())
			m_ObjectsStack.pop();
		m_ObjectsStack.push({ &m_Root });
	}
}

JSON_DEF_WRITE_SERIALIZATION_FN(std::string_view)
JSON_DEF_WRITE_SERIALIZATION_FN(int)
JSON_DEF_WRITE_SERIALIZATION_FN(float)

JSON_DEF_READ_SERIALIZATION_FN(std::string)
JSON_DEF_READ_SERIALIZATION_FN(int)
JSON_DEF_READ_SERIALIZATION_FN(float)

JSON_DEF_WRITE_SERIALIZATION_FN_ARRAY(std::string_view)
JSON_DEF_WRITE_SERIALIZATION_FN_ARRAY(int)
JSON_DEF_WRITE_SERIALIZATION_FN_ARRAY(float)

}
