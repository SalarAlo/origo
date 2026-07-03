#pragma once

#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "origo/core/RID.h"
#include "origo/serialization/ISerializer.h"
#include "origo/scripting/ComponentDescriptor.h"
#include "origo/scripting/ReflectedFieldStorage.h"

namespace Origo {

struct ComponentInstanceData {
	ComponentTypeID ID { UUID::bad() };
	ReflectedFieldStorage Fields {};
};

class ComponentDataManager {
public:
	ComponentDataManager();
	ComponentDataManager(const ComponentDataManager& other);
	ComponentDataManager& operator=(const ComponentDataManager& other);
	~ComponentDataManager();

	void add(RID entity, ComponentTypeID type);
	ComponentInstanceData* get(RID entity, ComponentTypeID type);
	const ComponentInstanceData* get(RID entity, ComponentTypeID type) const;
	const ReflectedFieldValue* find_field(RID entity, std::string_view name) const;
	bool has(RID entity, ComponentTypeID type) const;
	void copy_components(RID source, RID target);
	void serialize_entity(RID entity, ISerializer& backend) const;
	void deserialize_entity(RID entity, ISerializer& backend);
	void remove(RID entity, ComponentTypeID type);
	bool remove_if_exists(RID entity, ComponentTypeID type);
	void remove_all_components(RID entity);
	void migrate_component(RID entity, ComponentTypeID type);
	const std::unordered_map<RID, std::vector<ComponentInstanceData>>& get_all() const { return m_data; }

private:
	void subscribe_to_registry_updates();
	void unsubscribe_from_registry_updates();

	std::unordered_map<RID, std::vector<ComponentInstanceData>> m_data {};
	std::size_t m_component_updated_listener {};
	std::size_t m_component_removed_listener {};
	bool m_has_component_updated_listener {};
	bool m_has_component_removed_listener {};
};

}
