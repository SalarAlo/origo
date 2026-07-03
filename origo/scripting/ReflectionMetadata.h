#pragma once

#include <string>
#include <vector>

#include "origo/core/UUID.h"
#include "origo/scripting/Variant.h"

namespace Origo {

using TypeID = UUID;
using FieldID = UUID;
using FunctionID = UUID;
using RuntimeTypeHandle = UUID;
using RuntimeFunctionHandle = UUID;

enum class ReflectedTypeKind {
	Bool,
	Int,
	Float,
	String,
	Enum,
	Struct,
	Vec2,
	Vec3,
	Vec4,
	Color,
	Entity,
	Asset
};

enum class LifecycleKind {
	None,
	OnStart,
	OnUpdate,
	OnFixedUpdate,
	OnDestroy,
	OnCollision,
	OnTriggerEnter,
	OnTriggerExit
};

struct TypeRef {
	ReflectedTypeKind Kind { ReflectedTypeKind::Int };
	TypeID Type { UUID::bad() };
	std::string Name {};
};

struct AnnotationMetadata {
	std::string Name {};
	std::vector<std::string> Arguments {};
};

struct FieldMetadata {
	FieldID ID { UUID::bad() };
	std::string Name {};
	TypeRef Type {};
	Variant DefaultValue { 0 };
	std::vector<AnnotationMetadata> Annotations {};
	bool Serialized { true };
	bool InspectorVisible { true };
};

struct FunctionMetadata {
	FunctionID ID { UUID::bad() };
	std::string Name {};
	std::vector<TypeRef> Parameters {};
	TypeRef ReturnType {};
	LifecycleKind Lifecycle { LifecycleKind::None };
	RuntimeFunctionHandle RuntimeHandle { UUID::bad() };
	std::string BodySource {};
	std::vector<AnnotationMetadata> Annotations {};
};

struct ComponentMetadata {
	TypeID ID { UUID::bad() };
	UUID AssetID { UUID::bad() };
	std::string QualifiedName {};
	uint32_t Version {};
	std::vector<FieldMetadata> Fields {};
	std::vector<AnnotationMetadata> Annotations {};
};

struct ScriptMetadata {
	TypeID ID { UUID::bad() };
	UUID AssetID { UUID::bad() };
	std::string QualifiedName {};
	TypeRef ComponentType {};
	uint32_t Version {};
	RuntimeTypeHandle RuntimeType { UUID::bad() };
	std::vector<FieldMetadata> Fields {};
	std::vector<FunctionMetadata> Functions {};
	std::vector<AnnotationMetadata> Annotations {};
};

struct StructMetadata {
	TypeID ID { UUID::bad() };
	UUID AssetID { UUID::bad() };
	std::string QualifiedName {};
	uint32_t Version {};
	std::vector<FieldMetadata> Fields {};
	std::vector<AnnotationMetadata> Annotations {};
};

struct EnumValueMetadata {
	std::string Name {};
	int Value {};
};

struct EnumMetadata {
	TypeID ID { UUID::bad() };
	UUID AssetID { UUID::bad() };
	std::string QualifiedName {};
	uint32_t Version {};
	std::vector<EnumValueMetadata> Values {};
	std::vector<AnnotationMetadata> Annotations {};
};

struct NoctModuleMetadata {
	UUID AssetID { UUID::bad() };
	uint64_t ModuleVersion {};
	std::vector<ComponentMetadata> Components {};
	std::vector<ScriptMetadata> Scripts {};
	std::vector<StructMetadata> Structs {};
	std::vector<EnumMetadata> Enums {};
};

inline VariantType variant_type_from_reflected_type(ReflectedTypeKind kind) {
	switch (kind) {
	case ReflectedTypeKind::Bool:
		return VariantType::Bool;
	case ReflectedTypeKind::Int:
		return VariantType::Int;
	case ReflectedTypeKind::Float:
		return VariantType::Float;
	case ReflectedTypeKind::String:
		return VariantType::String;
	case ReflectedTypeKind::Enum:
		return VariantType::Enum;
	case ReflectedTypeKind::Vec2:
		return VariantType::Vec2;
	case ReflectedTypeKind::Vec3:
		return VariantType::Vec3;
	case ReflectedTypeKind::Vec4:
	case ReflectedTypeKind::Color:
		return VariantType::Vec4;
	case ReflectedTypeKind::Entity:
		return VariantType::Entity;
	case ReflectedTypeKind::Asset:
		return VariantType::Asset;
	case ReflectedTypeKind::Struct:
		return VariantType::Struct;
	}

	return VariantType::Int;
}

}
