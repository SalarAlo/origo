#include "origo/assets/UniformList.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Origo {

template <>
UniformType Uniform<int>::GetUniformType() const { return UniformType::Int; }

template <>
UniformType Uniform<float>::GetUniformType() const { return UniformType::Float; }

template <>
UniformType Uniform<glm::vec2>::GetUniformType() const { return UniformType::Vec2; }

template <>
UniformType Uniform<glm::vec3>::GetUniformType() const { return UniformType::Vec3; }

template <>
UniformType Uniform<glm::mat4>::GetUniformType() const { return UniformType::Mat4; }

template <>
nlohmann::json Uniform<int>::Serialize() const {
	return { { "type", "int" }, { "value", m_Value } };
}

template <>
nlohmann::json Uniform<float>::Serialize() const {
	return { { "type", "float" }, { "value", m_Value } };
}

template <>
nlohmann::json Uniform<glm::vec2>::Serialize() const {
	return { { "type", "vec2" }, { "value", { m_Value.x, m_Value.y } } };
}

template <>
nlohmann::json Uniform<glm::vec3>::Serialize() const {
	return { { "type", "vec3" }, { "value", { m_Value.x, m_Value.y, m_Value.z } } };
}

template <>
nlohmann::json Uniform<glm::mat4>::Serialize() const {
	const float* ptr = glm::value_ptr(m_Value);
	std::vector<float> values(ptr, ptr + 16);
	return { { "type", "mat4" }, { "value", values } };
}

}
