#include "origo/scripting/TypeRegistry.h"

#include "origo/core/Logger.h"

namespace Origo {

void TypeRegistry::replace_module(const NoctModuleMetadata& metadata) {
	std::vector<ComponentDescriptor> components;
	components.reserve(metadata.Components.size());
	for (const auto& component : metadata.Components)
		components.push_back(ComponentRegistry::create_descriptor(component));

	std::vector<ScriptDescriptor> scripts;
	scripts.reserve(metadata.Scripts.size());
	for (const auto& script : metadata.Scripts)
		scripts.push_back(ScriptRegistry::create_descriptor(script));

	StructRegistry::replace_asset_structs(metadata.AssetID, metadata.Structs);
	EnumRegistry::replace_asset_enums(metadata.AssetID, metadata.Enums);
	ComponentRegistry::replace_asset_components(metadata.AssetID, std::move(components));
	ScriptRegistry::replace_asset_scripts(metadata.AssetID, std::move(scripts));

	for (const auto& [_, script] : ScriptRegistry::get_all()) {
		if (script.AssetID != metadata.AssetID || script.ComponentType.Type == UUID::bad())
			continue;

		if (!ComponentRegistry::exists(script.ComponentType.Type)) {
			const std::string component_name = script.ComponentType.Name.empty() ? script.ComponentType.Type.to_string() : script.ComponentType.Name;
			if (!script.ComponentType.Name.empty()) {
				if (auto same_name = ComponentRegistry::try_find_by_name(script.ComponentType.Name)) {
					ORG_CORE_WARN("[Noct] Script '{}' declares component association <{}>, but its component type id {} does not match the registered component id {}. It will not auto-attach or run.",
					    script.Name,
					    script.ComponentType.Name,
					    script.ComponentType.Type.to_string(),
					    same_name->to_string());
					continue;
				}
			}

			ORG_CORE_WARN("[Noct] Script '{}' declares component association <{}>, but that component type is not registered. The script will not auto-attach or run until the component metadata exists.",
			    script.Name,
			    component_name);
		}
	}
}

}
