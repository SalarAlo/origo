#include <limits>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "origo/scripting/ScriptSystem.h"

#include "origo/core/Logger.h"
#include "origo/core/PathContext.h"

#include "origo/scripting/ScriptComponentRegistry.h"

#include "Runtime.h"

namespace Origo {

namespace {

VariantType variant_type_from_noct(Noct::NoctFieldType type) {
	switch (type) {
	case Noct::NoctFieldType::Int:
		return VariantType::Int;
	case Noct::NoctFieldType::Float:
		return VariantType::Float;
	case Noct::NoctFieldType::Bool:
		return VariantType::Bool;
	case Noct::NoctFieldType::String:
		return VariantType::String;
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
	}

	throw std::runtime_error("Unknown Noct field type");
}

std::vector<ScriptFieldDeclaration> fields_from_noct(const Noct::NoctComponentDeclaration& component) {
	std::vector<ScriptFieldDeclaration> fields;
	fields.reserve(component.Fields.size());

	for (const auto& field : component.Fields) {
		fields.push_back(ScriptFieldDeclaration {
		    field.Name,
		    variant_type_from_noct(field.Type),
		    variant_from_noct(field.DefaultValue, field.Type),
		});
	}

	return fields;
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

bool source_contains_component_declaration(std::string_view source) {
	return source.find("Component.define") != std::string_view::npos;
}

bool script_has_registered_components(const UUID& script_id) {
	for (const auto& [_, descriptor] : ScriptComponentRegistry::get_all()) {
		if (descriptor.ScriptID == script_id)
			return true;
	}

	return false;
}

}

class NoctScriptRuntime {
public:
	void initialise() {
		Noct::RuntimeConfig config {};
		config.ProjectRoot = PathContextService::get_instance().project().Root;

		m_runtime = std::make_unique<Noct::Runtime>(std::move(config));
		m_runtime->initialize();
		m_runtime->register_native_module("Component");
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

		auto declarations = m_runtime->extract_component_declarations(compile.script());
		log_noct_diagnostics(entry.Path, declarations.diagnostics());

		if (!declarations)
			return;

		if (declarations.components().empty() && source_contains_component_declaration(entry.Source) && script_has_registered_components(id)) {
			ORG_CORE_ERROR("[Noct] '{}' still contains Component.define but produced no component declarations. Keeping previous script components.",
			    entry.Path.string());
			return;
		}

		ORG_CORE_TRACE("[Noct] '{}' produced {} component declaration(s).",
		    entry.Path.string(),
		    declarations.components().size());

		std::vector<ScriptComponentDescriptor> descriptors;
		descriptors.reserve(declarations.components().size());

		for (const auto& component : declarations.components()) {
			try {
				descriptors.push_back(ScriptComponentRegistry::create_descriptor(
				    id,
				    component.Name,
				    fields_from_noct(component)));
			} catch (const std::exception& e) {
				ORG_CORE_ERROR("[Noct] Failed to create component descriptor '{}' from '{}': {}",
				    component.Name,
				    entry.Path.string(),
				    e.what());
			}
		}

		ScriptComponentRegistry::replace_script_components(id, std::move(descriptors));
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

}
