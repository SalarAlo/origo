#pragma once

#include <variant>
#include <cassert>

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
	String
};

class Variant {
public:
	using StdVariantType = std::variant<int, bool, float, std::string>;

public:
	ORIGO_VARIANT_CTOR(bool, VariantType::Bool)
	ORIGO_VARIANT_CTOR(int, VariantType::Int)
	ORIGO_VARIANT_CTOR(float, VariantType::Float)
	ORIGO_VARIANT_CTOR(const std::string&, VariantType::String)

	ORIGO_VARIANT_GETTER(Bool, bool, VariantType::Bool)
	ORIGO_VARIANT_GETTER(Int, int, VariantType::Int)
	ORIGO_VARIANT_GETTER(Float, float, VariantType::Float)
	ORIGO_VARIANT_GETTER(String, std::string, VariantType::String)

	VariantType GetType() const { return m_Type; }

private:
	StdVariantType m_Value;
	VariantType m_Type;
};

}
