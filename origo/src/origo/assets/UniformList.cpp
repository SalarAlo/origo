#include <limits>

#include "origo/assets/UniformList.hpp"

#include "magic_enum/magic_enum.hpp"

#include "origo/core/Logger.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

#pragma region TYPE MAPPING

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
#pragma endregion

#pragma region SERIALIZATION

template <>
void Uniform<int>::serialize(ISerializer& backend) const {
	backend.write("type", magic_enum::enum_name(get_uniform_type()));
	backend.write("value", m_value);
}

template <>
void Uniform<unsigned int>::serialize(ISerializer& backend) const {
	backend.write("type", magic_enum::enum_name(get_uniform_type()));
	if (m_value > static_cast<unsigned int>(std::numeric_limits<int>::max())) {
		ORG_ERROR("Uniform<unsigned int> value too large for serializer int");
		backend.write("value", std::numeric_limits<int>::max());
		return;
	}
	backend.write("value", static_cast<int>(m_value));
}

template <>
void Uniform<bool>::serialize(ISerializer& backend) const {
	backend.write("type", magic_enum::enum_name(get_uniform_type()));
	backend.write("value", m_value ? 1 : 0);
}

template <>
void Uniform<float>::serialize(ISerializer& backend) const {
	backend.write("type", magic_enum::enum_name(get_uniform_type()));
	backend.write("value", m_value);
}

template <>
void Uniform<Vec2>::serialize(ISerializer& backend) const {
	backend.write("type", magic_enum::enum_name(get_uniform_type()));
	backend.write("x", m_value.x);
	backend.write("y", m_value.y);
}

template <>
void Uniform<Vec3>::serialize(ISerializer& backend) const {
	backend.write("type", magic_enum::enum_name(get_uniform_type()));
	backend.write("x", m_value.x);
	backend.write("y", m_value.y);
	backend.write("z", m_value.z);
}

template <>
void Uniform<glm::mat4>::serialize(ISerializer& backend) const {
	backend.write("type", magic_enum::enum_name(get_uniform_type()));

	backend.begin_array("value");
	for (int c = 0; c < 4; ++c) {
		for (int r = 0; r < 4; ++r) {
			backend.write(m_value[c][r]);
		}
	}
	backend.end_array();
}
#pragma endregion

#pragma region DESERIALIZATION

Scope<UniformBase> UniformBase::deserialize(ISerializer& backend) {
	std::string type_str;
	if (!backend.try_read("type", type_str))
		return nullptr;

	auto type_opt = magic_enum::enum_cast<UniformType>(type_str);
	if (!type_opt)
		return nullptr;

	switch (*type_opt) {
	case UniformType::Int: {
		int v {};
		if (!backend.try_read("value", v))
			return nullptr;
		auto u = MakeScope<Uniform<int>>(v);
		return u;
	}

	case UniformType::UnsignedInt: {
		int v {};
		if (!backend.try_read("value", v))
			return nullptr;
		if (v < 0)
			return nullptr;
		auto u = MakeScope<Uniform<unsigned int>>(static_cast<unsigned int>(v));
		return u;
	}

	case UniformType::Float: {
		float v {};
		if (!backend.try_read("value", v))
			return nullptr;
		auto u = MakeScope<Uniform<float>>(v);
		return u;
	}

	case UniformType::Bool: {
		int v {};
		if (!backend.try_read("value", v))
			return nullptr;
		auto u = MakeScope<Uniform<bool>>(v);
		return u;
	}

	case UniformType::Vec2: {
		float x {}, y {};
		if (!backend.try_read("x", x))
			return nullptr;
		if (!backend.try_read("y", y))
			return nullptr;
		auto u = MakeScope<Uniform<Vec2>>(Vec2 { x, y });
		return u;
	}

	case UniformType::Vec3: {
		float x {}, y {}, z {};
		if (!backend.try_read("x", x))
			return nullptr;
		if (!backend.try_read("y", y))
			return nullptr;
		if (!backend.try_read("z", z))
			return nullptr;
		auto u = MakeScope<Uniform<Vec3>>(Vec3 { x, y, z });
		return u;
	}

	case UniformType::Mat4: {
		glm::mat4 m(1.0f);

		backend.begin_array("value");

		for (int c = 0; c < 4; ++c) {
			for (int r = 0; r < 4; ++r) {
				float v {};
				if (!backend.try_read_array_element(v)) {
					backend.end_array();
					return nullptr;
				}
				m[c][r] = v;
			}
		}

		backend.end_array();

		auto u = MakeScope<Uniform<glm::mat4>>(m);
		return u;
	}
	}

	return nullptr;
}
#pragma endregion

}
