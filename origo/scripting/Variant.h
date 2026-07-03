#pragma once

#include <variant>
#include <cassert>

#include "origo/core/RID.h"
#include "origo/core/Typedefs.h"
#include "origo/core/UUID.h"

#define ORIGO_VARIANT_CTOR(TYPE, ENUM) \
	Variant(TYPE v)                \
	    : m_Value(v)               \
	    , m_Type(ENUM) { }

#define ORIGO_VARIANT_GETTER(SUFFIX, TYPE, ENUM) \
	bool TryGetAs##SUFFIX(TYPE& out) const { \
		if (m_Type != ENUM)              \
			return false;            \
		out = std::get<TYPE>(m_Value);   \
		return true;                     \
	}

namespace Origo {

enum class VariantType {
	Int,
	Bool,
	Float,
	String,
	Enum,
	Vec2,
	Vec3,
	Vec4,
	Entity,
	Asset,
	Struct
};

class Variant {
public:
	using StdVariantType = std::variant<std::monostate, int, bool, float, std::string, Vec2, Vec3, glm::vec4, RID, UUID>;

public:
	ORIGO_VARIANT_CTOR(bool, VariantType::Bool)
	ORIGO_VARIANT_CTOR(int, VariantType::Int)
	ORIGO_VARIANT_CTOR(float, VariantType::Float)
	ORIGO_VARIANT_CTOR(const std::string&, VariantType::String)
	Variant(const char* v)
	    : m_Value(std::string(v))
	    , m_Type(VariantType::String) { }
	ORIGO_VARIANT_CTOR(const Vec2&, VariantType::Vec2)
	ORIGO_VARIANT_CTOR(const Vec3&, VariantType::Vec3)
	ORIGO_VARIANT_CTOR(const glm::vec4&, VariantType::Vec4)
	ORIGO_VARIANT_CTOR(const RID&, VariantType::Entity)
	ORIGO_VARIANT_CTOR(const UUID&, VariantType::Asset)
	explicit Variant(VariantType type)
	    : m_Value(std::monostate {})
	    , m_Type(type) { }

	Variant(int v, VariantType type)
	    : m_Value(v)
	    , m_Type(type) { }

	ORIGO_VARIANT_GETTER(Bool, bool, VariantType::Bool)
	ORIGO_VARIANT_GETTER(Int, int, VariantType::Int)
	ORIGO_VARIANT_GETTER(Float, float, VariantType::Float)
	ORIGO_VARIANT_GETTER(String, std::string, VariantType::String)
	ORIGO_VARIANT_GETTER(Vec2, Vec2, VariantType::Vec2)
	ORIGO_VARIANT_GETTER(Vec3, Vec3, VariantType::Vec3)
	ORIGO_VARIANT_GETTER(Vec4, glm::vec4, VariantType::Vec4)
	ORIGO_VARIANT_GETTER(Entity, RID, VariantType::Entity)
	ORIGO_VARIANT_GETTER(Asset, UUID, VariantType::Asset)

	bool try_get_as_enum(int& out) const {
		if (m_Type != VariantType::Enum)
			return false;
		out = std::get<int>(m_Value);
		return true;
	}

	VariantType get_type() const { return m_Type; }

private:
	StdVariantType m_Value;
	VariantType m_Type;
};

}
