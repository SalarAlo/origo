#include <limits>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "origo/scripting/ScriptSystem.h"

#include "origo/core/Logger.h"
#include "origo/core/PathContext.h"

#include "origo/scripting/TypeRegistry.h"

#include "Runtime.h"

namespace Origo {

namespace {

ReflectedTypeKind reflected_type_from_noct(Noct::NoctFieldType type) {
	switch (type) {
	case Noct::NoctFieldType::Int:
		return ReflectedTypeKind::Int;
	case Noct::NoctFieldType::Float:
		return ReflectedTypeKind::Float;
	case Noct::NoctFieldType::Bool:
		return ReflectedTypeKind::Bool;
	case Noct::NoctFieldType::String:
		return ReflectedTypeKind::String;
	case Noct::NoctFieldType::Enum:
		return ReflectedTypeKind::Enum;
	case Noct::NoctFieldType::Vec2:
		return ReflectedTypeKind::Vec2;
	case Noct::NoctFieldType::Vec3:
		return ReflectedTypeKind::Vec3;
	case Noct::NoctFieldType::Vec4:
		return ReflectedTypeKind::Vec4;
	case Noct::NoctFieldType::Color:
		return ReflectedTypeKind::Color;
	case Noct::NoctFieldType::Entity:
		return ReflectedTypeKind::Entity;
	case Noct::NoctFieldType::Asset:
		return ReflectedTypeKind::Asset;
	case Noct::NoctFieldType::Struct:
		return ReflectedTypeKind::Struct;
	}

	throw std::runtime_error("Unknown Noct field type");
}

Variant variant_from_noct(const Noct::NoctValue& value, Noct::NoctFieldType type) {
	switch (type) {
	case Noct::NoctFieldType::Int: {
		const auto raw = std::get<int64_t>(value);
		if (raw < std::numeric_limits<int>::min() || raw > std::numeric_limits<int>::max())
			throw std::runtime_error("Noct int default value is outside Origo int range");
		return Variant(static_cast<int>(raw));
	}
	case Noct::NoctFieldType::Float:
		return Variant(static_cast<float>(std::get<double>(value)));
	case Noct::NoctFieldType::Bool:
		return Variant(std::get<bool>(value));
	case Noct::NoctFieldType::String:
		return Variant(std::get<std::string>(value));
	case Noct::NoctFieldType::Enum:
		if (const auto* raw = std::get_if<int64_t>(&value))
			return Variant(static_cast<int>(*raw), VariantType::Enum);
		return Variant(0, VariantType::Enum);
	case Noct::NoctFieldType::Vec2:
		return Variant(Vec2 { 0.0f });
	case Noct::NoctFieldType::Vec3:
		return Variant(Vec3 { 0.0f });
	case Noct::NoctFieldType::Vec4:
	case Noct::NoctFieldType::Color:
		return Variant(glm::vec4 { 0.0f });
	case Noct::NoctFieldType::Entity:
		return Variant(RID {});
	case Noct::NoctFieldType::Asset:
		return Variant(UUID::bad());
	case Noct::NoctFieldType::Struct:
		return Variant(VariantType::Struct);
	}

	throw std::runtime_error("Unknown Noct field type");
}

LifecycleKind lifecycle_from_noct(Noct::NoctLifecycleKind lifecycle) {
	switch (lifecycle) {
	case Noct::NoctLifecycleKind::OnStart:
		return LifecycleKind::OnStart;
	case Noct::NoctLifecycleKind::OnUpdate:
		return LifecycleKind::OnUpdate;
	case Noct::NoctLifecycleKind::OnFixedUpdate:
		return LifecycleKind::OnFixedUpdate;
	case Noct::NoctLifecycleKind::OnDestroy:
		return LifecycleKind::OnDestroy;
	case Noct::NoctLifecycleKind::OnCollision:
		return LifecycleKind::OnCollision;
	case Noct::NoctLifecycleKind::OnTriggerEnter:
		return LifecycleKind::OnTriggerEnter;
	case Noct::NoctLifecycleKind::OnTriggerExit:
		return LifecycleKind::OnTriggerExit;
	case Noct::NoctLifecycleKind::None:
		return LifecycleKind::None;
	}
	return LifecycleKind::None;
}

UUID uuid_from_noct_id(const std::string& id) {
	return UUID::from_hash(id);
}

TypeRef type_ref_from_noct(const Noct::NoctTypeRef& type) {
	return TypeRef {
		.Kind = reflected_type_from_noct(type.Kind),
		.Type = type.TypeID.empty() ? UUID::bad() : uuid_from_noct_id(type.TypeID),
		.Name = type.Name
	};
}

std::vector<AnnotationMetadata> annotations_from_noct(const std::vector<Noct::NoctAnnotationMetadata>& annotations) {
	std::vector<AnnotationMetadata> out;
	out.reserve(annotations.size());
	for (const auto& annotation : annotations) {
		out.push_back(AnnotationMetadata {
		    .Name = annotation.Name,
		    .Arguments = annotation.Arguments });
	}
	return out;
}

FieldMetadata field_metadata_from_noct(const Noct::NoctFieldMetadata& field) {
	FieldMetadata out {};
	out.ID = uuid_from_noct_id(field.ID);
	out.Name = field.Name;
	out.Type = type_ref_from_noct(field.Type);
	out.DefaultValue = variant_from_noct(field.DefaultValue, field.Type.Kind);
	out.Annotations = annotations_from_noct(field.Annotations);
	out.Serialized = field.Serialized;
	out.InspectorVisible = field.InspectorVisible;
	return out;
}

ComponentMetadata component_metadata_from_noct(const Noct::NoctComponentMetadata& component) {
	ComponentMetadata out {};
	out.ID = uuid_from_noct_id(component.ID);
	out.AssetID = uuid_from_noct_id(component.SourceScript.Value);
	out.QualifiedName = component.QualifiedName;
	out.Version = component.Version;
	out.Annotations = annotations_from_noct(component.Annotations);
	out.Fields.reserve(component.Fields.size());
	for (const auto& field : component.Fields)
		out.Fields.push_back(field_metadata_from_noct(field));
	return out;
}

ScriptMetadata script_metadata_from_noct(const Noct::NoctScriptMetadata& script) {
	ScriptMetadata out {};
	out.ID = uuid_from_noct_id(script.ID);
	out.AssetID = uuid_from_noct_id(script.SourceScript.Value);
	out.QualifiedName = script.QualifiedName;
	out.ComponentType = type_ref_from_noct(script.ComponentType);
	out.Version = script.Version;
	out.RuntimeType = uuid_from_noct_id(script.RuntimeType);
	out.Annotations = annotations_from_noct(script.Annotations);
	out.Fields.reserve(script.Fields.size());
	for (const auto& field : script.Fields)
		out.Fields.push_back(field_metadata_from_noct(field));
	out.Functions.reserve(script.Functions.size());
	for (const auto& fn : script.Functions) {
		FunctionMetadata function {};
		function.ID = uuid_from_noct_id(fn.ID);
		function.Name = fn.Name;
		function.Lifecycle = lifecycle_from_noct(fn.Lifecycle);
		function.RuntimeHandle = uuid_from_noct_id(fn.RuntimeHandle);
		function.BodySource = fn.BodySource;
		function.Annotations = annotations_from_noct(fn.Annotations);
		for (const auto& param : fn.Parameters)
			function.Parameters.push_back(type_ref_from_noct(param));
		out.Functions.push_back(std::move(function));
	}
	return out;
}

StructMetadata struct_metadata_from_noct(const Noct::NoctStructMetadata& structure) {
	StructMetadata out {};
	out.ID = uuid_from_noct_id(structure.ID);
	out.AssetID = uuid_from_noct_id(structure.SourceScript.Value);
	out.QualifiedName = structure.QualifiedName;
	out.Version = structure.Version;
	out.Annotations = annotations_from_noct(structure.Annotations);
	for (const auto& field : structure.Fields)
		out.Fields.push_back(field_metadata_from_noct(field));
	return out;
}

EnumMetadata enum_metadata_from_noct(const Noct::NoctEnumMetadata& e) {
	EnumMetadata out {};
	out.ID = uuid_from_noct_id(e.ID);
	out.AssetID = uuid_from_noct_id(e.SourceScript.Value);
	out.QualifiedName = e.QualifiedName;
	out.Version = e.Version;
	out.Annotations = annotations_from_noct(e.Annotations);
	for (const auto& value : e.Values) {
		out.Values.push_back(EnumValueMetadata {
		    .Name = value.Name,
		    .Value = static_cast<int>(value.Value) });
	}
	return out;
}

NoctModuleMetadata module_metadata_from_noct(const UUID& asset_id, const Noct::NoctModuleMetadata& module) {
	NoctModuleMetadata out {};
	out.AssetID = asset_id;
	out.ModuleVersion = module.ModuleVersion;
	out.Components.reserve(module.Components.size());
	for (const auto& component : module.Components) {
		auto converted = component_metadata_from_noct(component);
		converted.AssetID = asset_id;
		out.Components.push_back(std::move(converted));
	}
	out.Scripts.reserve(module.Scripts.size());
	for (const auto& script : module.Scripts) {
		auto converted = script_metadata_from_noct(script);
		converted.AssetID = asset_id;
		out.Scripts.push_back(std::move(converted));
	}
	out.Structs.reserve(module.Structs.size());
	for (const auto& structure : module.Structs) {
		auto converted = struct_metadata_from_noct(structure);
		converted.AssetID = asset_id;
		out.Structs.push_back(std::move(converted));
	}
	out.Enums.reserve(module.Enums.size());
	for (const auto& e : module.Enums) {
		auto converted = enum_metadata_from_noct(e);
		converted.AssetID = asset_id;
		out.Enums.push_back(std::move(converted));
	}
	return out;
}

void log_noct_diagnostics(const std::filesystem::path& script_path, const std::vector<Noct::NoctDiagnostic>& diagnostics) {
	for (const auto& diagnostic : diagnostics) {
		const auto& location = diagnostic.Location;
		const std::filesystem::path file = location.File.empty() ? script_path : location.File;
		const std::string code = diagnostic.Code.empty() ? "" : " [" + diagnostic.Code + "]";
		const std::string message = file.string() + ":" + std::to_string(location.Line) + ":" + std::to_string(location.Column) + code + " " + diagnostic.Message;

		switch (diagnostic.Severity) {
		case Noct::NoctSeverity::Info:
			ORG_CORE_INFO("[Noct] {}", message);
			break;
		case Noct::NoctSeverity::Warning:
			ORG_CORE_WARN("[Noct] {}", message);
			break;
		case Noct::NoctSeverity::Error:
			ORG_CORE_ERROR("[Noct] {}", message);
			break;
		}
	}
}

}

class NoctScriptRuntime {
public:
	void initialise() {
		Noct::RuntimeConfig config {};
		config.ProjectRoot = PathContextService::get_instance().project().Root;

		m_runtime = std::make_unique<Noct::Runtime>(std::move(config));
		m_runtime->initialize();
		m_runtime->add_module_search_path(PathContextService::get_instance().project().Root);
	}

	void shutdown() {
		if (m_runtime)
			m_runtime->shutdown();
		m_runtime.reset();
	}

	void execute(const ScriptSystem::ScriptEntry& entry, const UUID& id) {
		if (!m_runtime)
			initialise();

		const Noct::ScriptId script_id { id.to_string() };
		auto compile = m_runtime->compile_source(script_id, entry.Path, entry.Source);
		log_noct_diagnostics(entry.Path, compile.diagnostics());

		if (!compile)
			return;

		auto reflection = m_runtime->extract_reflection_metadata(compile.script());
		log_noct_diagnostics(entry.Path, reflection.diagnostics());

		if (!reflection)
			return;

		ScriptSystem::ingest_metadata(module_metadata_from_noct(id, reflection.metadata()));
	}

	Noct::Runtime* runtime() {
		if (!m_runtime)
			initialise();
		return m_runtime.get();
	}

private:
	std::unique_ptr<Noct::Runtime> m_runtime {};
};

std::unique_ptr<NoctScriptRuntime> ScriptSystem::s_noct_runtime {};

void ScriptSystem::initialise_state() {
	if (s_initialised)
		return;

	s_initialised = true;
	s_noct_runtime = std::make_unique<NoctScriptRuntime>();
	s_noct_runtime->initialise();
}

void ScriptSystem::shutdown() {
	if (s_noct_runtime)
		s_noct_runtime->shutdown();

	s_noct_runtime.reset();
	s_initialised = false;
}

void ScriptSystem::register_script(const UUID& id, const std::filesystem::path& path, const std::string& source) {
	if (s_scripts.contains(id)) {
		auto& entry = s_scripts[id];
		entry.Path = path;
		entry.Source = source;
		entry.ReloadNecessary = true;
		return;
	}

	ScriptEntry entry {};
	entry.Source = source;
	entry.Path = path;
	s_scripts.emplace(id, entry);
}

void ScriptSystem::reload_all() {
	shutdown();
	initialise_state();

	for (auto& [id, entry] : s_scripts) {
		execute(entry, id);
		entry.ReloadNecessary = false;
	}
}

void ScriptSystem::reload_all_necessary() {
	shutdown();
	initialise_state();

	for (auto& [id, entry] : s_scripts) {
		if (!entry.ReloadNecessary)
			continue;

		execute(entry, id);
		entry.ReloadNecessary = false;
	}
}

void ScriptSystem::execute(const ScriptEntry& entry, const UUID& id) {
	initialise_state();
	s_noct_runtime->execute(entry, id);
}

void ScriptSystem::ingest_metadata(const NoctModuleMetadata& metadata) {
	TypeRegistry::replace_module(metadata);
}

Noct::Runtime* ScriptSystem::runtime() {
	initialise_state();
	return s_noct_runtime ? s_noct_runtime->runtime() : nullptr;
}

}
