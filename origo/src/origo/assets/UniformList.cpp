#include "origo/assets/UniformList.hpp"

#include "origo/core/Logger.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

template <>
UniformType Uniform<bool>::GetUniformType() const { return UniformType::Bool; }

template <>
UniformType Uniform<int>::GetUniformType() const { return UniformType::Int; }

template <>
UniformType Uniform<float>::GetUniformType() const { return UniformType::Float; }
template <>
UniformType Uniform<unsigned int>::GetUniformType() const { return UniformType::UnsignedInt; }

template <>
UniformType Uniform<Vec2>::GetUniformType() const { return UniformType::Vec2; }

template <>
UniformType Uniform<Vec3>::GetUniformType() const { return UniformType::Vec3; }

template <>
UniformType Uniform<glm::mat4>::GetUniformType() const { return UniformType::Mat4; }

template <>
void Uniform<int>::Serialize(ISerializer& backend) const {
	backend.Write("type", "int");
	backend.Write("value", m_Value);
};

template <>
void Uniform<bool>::Serialize(ISerializer& backend) const {
	backend.Write("type", "bool");
	backend.Write("value", m_Value ? 1 : 0);
};

template <>
void Uniform<float>::Serialize(ISerializer& backend) const {
	backend.Write("type", "float");
	backend.Write("value", m_Value);
}

template <>
void Uniform<Vec2>::Serialize(ISerializer& backend) const {
	backend.Write("type", "vec2");
	backend.Write("x", m_Value.x);
	backend.Write("y", m_Value.y);
}

template <>
void Uniform<Vec3>::Serialize(ISerializer& backend) const {
	backend.Write("type", "vec3");
	backend.Write("x", m_Value.x);
	backend.Write("y", m_Value.y);
	backend.Write("z", m_Value.z);
}

template <>
void Uniform<glm::mat4>::Serialize(ISerializer& backend) const {
	// TODO: implement this shit. i hate modularisation
	ORG_ERROR("To be continued...");
}

}
