#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "origo/core/Action.h"
#include "origo/scripting/ComponentDescriptor.h"

namespace Origo {

class ComponentRegistry {
public:
	static ComponentTypeID register_or_update(ComponentDescriptor descriptor);
	static ComponentDescriptor create_descriptor(const ComponentMetadata& metadata);
	static std::vector<ComponentTypeID> replace_asset_components(const UUID& asset_id, std::vector<ComponentDescriptor> descriptors);

	static const ComponentDescriptor& get(ComponentTypeID id);
	static std::optional<ComponentTypeID> try_find_by_name(const std::string& name);
	static std::optional<ComponentTypeID> try_find_by_asset_and_name(const UUID& asset_id, const std::string& name);
	static std::vector<ComponentTypeID> find_by_asset(const UUID& asset_id);
	static bool exists(ComponentTypeID id);

	static const std::unordered_map<ComponentTypeID, ComponentDescriptor>& get_all() { return s_descriptors; }
	static Action<void, ComponentTypeID>& on_component_updated();
	static Action<void, ComponentTypeID>& on_component_removed();

private:
	inline static std::unordered_map<ComponentTypeID, ComponentDescriptor> s_descriptors {};
	inline static std::unordered_map<std::string, ComponentTypeID> s_name_to_component_id {};
};

}
