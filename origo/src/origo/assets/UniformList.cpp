#include "origo/assets/UniformList.hpp"

#include "origo/core/Logger.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

template <>
UniformType Uniform<bool>::get_uniform_type() const { return UniformType::Bool; }

template <>
UniformType Uniform<int>::get_uniform_type() const { return UniformType::Int; }

template <>
UniformType Uniform<float>::get_uniform_type() const { return UniformType::Float; }
template <>
UniformType Uniform<unsigned int>::get_uniform_type() const { return UniformType::UnsignedInt; }

template <>
UniformType Uniform<Vec2>::get_uniform_type() const { return UniformType::Vec2; }

template <>
UniformType Uniform<Vec3>::get_uniform_type() const { return UniformType::Vec3; }

template <>
UniformType Uniform<glm::mat4>::get_uniform_type() const { return UniformType::Mat4; }

template <>
void Uniform<int>::serialize(ISerializer& backend) const {
	backend.write("type", "int");
	backend.write("value", m_value);
};

template <>
void Uniform<bool>::serialize(ISerializer& backend) const {
	backend.write("type", "bool");
	backend.write("value", m_value ? 1 : 0);
};

template <>
void Uniform<float>::serialize(ISerializer& backend) const {
	backend.write("type", "float");
	backend.write("value", m_value);
}

template <>
void Uniform<Vec2>::serialize(ISerializer& backend) const {
	backend.write("type", "vec2");
	backend.write("x", m_value.x);
	backend.write("y", m_value.y);
}

template <>
void Uniform<Vec3>::serialize(ISerializer& backend) const {
	backend.write("type", "vec3");
	backend.write("x", m_value.x);
	backend.write("y", m_value.y);
	backend.write("z", m_value.z);
}

template <>
void Uniform<glm::mat4>::serialize(ISerializer& backend) const {
	// TODO: implement this shit. i hate modularisation
	ORG_ERROR("To be continued...");
}

}
