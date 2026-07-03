#include <algorithm>
#include <optional>

#include "origo/scripting/ComponentDataManager.h"

#include "origo/scripting/ComponentRegistry.h"

namespace Origo {

namespace {
	std::string variant_type_to_string(VariantType type) {
		switch (type) {
		case VariantType::Int:
			return "int";
		case VariantType::Bool:
			return "bool";
		case VariantType::Float:
			return "float";
		case VariantType::String:
			return "string";
		case VariantType::Vec3:
			return "vec3";
		case VariantType::Enum:
			return "enum";
		case VariantType::Vec2:
			return "vec2";
		case VariantType::Vec4:
			return "vec4";
		case VariantType::Entity:
			return "entity";
		case VariantType::Asset:
			return "asset";
		case VariantType::Struct:
			return "struct";
		}
		return "unknown";
	}

	std::optional<VariantType> variant_type_from_string(const std::string& type) {
		if (type == "int")
			return VariantType::Int;
		if (type == "bool")
			return VariantType::Bool;
		if (type == "float")
			return VariantType::Float;
		if (type == "string")
			return VariantType::String;
		if (type == "enum")
			return VariantType::Enum;
		if (type == "vec2")
			return VariantType::Vec2;
		if (type == "vec3")
			return VariantType::Vec3;
		if (type == "vec4")
			return VariantType::Vec4;
		if (type == "entity")
			return VariantType::Entity;
		if (type == "asset")
			return VariantType::Asset;
		return std::nullopt;
	}

	void serialize_variant_value(const Variant& value, ISerializer& backend) {
		switch (value.get_type()) {
		case VariantType::Int: {
			int v {};
			if (value.TryGetAsInt(v))
				backend.write("value", v);
			return;
		}
		case VariantType::Bool: {
			bool v {};
			if (value.TryGetAsBool(v))
				backend.write("value", v);
			return;
		}
		case VariantType::Float: {
			float v {};
			if (value.TryGetAsFloat(v))
				backend.write("value", v);
			return;
		}
		case VariantType::String: {
			std::string v;
			if (value.TryGetAsString(v))
				backend.write("value", v);
			return;
		}
		case VariantType::Enum: {
			int v {};
			if (value.try_get_as_enum(v))
				backend.write("value", v);
			return;
		}
		case VariantType::Vec2: {
			Vec2 v {};
			if (value.TryGetAsVec2(v)) {
				backend.write("x", v.x);
				backend.write("y", v.y);
			}
			return;
		}
		case VariantType::Vec3: {
			Vec3 v {};
			if (value.TryGetAsVec3(v))
				backend.write("value", v);
			return;
		}
		case VariantType::Vec4: {
			glm::vec4 v {};
			if (value.TryGetAsVec4(v)) {
				backend.write("x", v.x);
				backend.write("y", v.y);
				backend.write("z", v.z);
				backend.write("w", v.w);
			}
			return;
		}
		case VariantType::Entity: {
			RID v {};
			if (value.TryGetAsEntity(v))
				backend.write("value", v.to_string());
			return;
		}
		case VariantType::Asset: {
			UUID v { UUID::bad() };
			if (value.TryGetAsAsset(v))
				backend.write("value", v.to_string());
			return;
		}
		case VariantType::Struct:
			return;
		}
	}

	std::optional<Variant> deserialize_variant_value(VariantType type, ISerializer& backend) {
		switch (type) {
		case VariantType::Int: {
			int value {};
			if (!backend.try_read("value", value))
				return std::nullopt;
			return Variant(value);
		}
		case VariantType::Bool: {
			bool value {};
			if (!backend.try_read("value", value))
				return std::nullopt;
			return Variant(value);
		}
		case VariantType::Float: {
			float value {};
			if (!backend.try_read("value", value))
				return std::nullopt;
			return Variant(value);
		}
		case VariantType::String: {
			std::string value;
			if (!backend.try_read("value", value))
				return std::nullopt;
			return Variant(value);
		}
		case VariantType::Enum: {
			int value {};
			if (!backend.try_read("value", value))
				return std::nullopt;
			return Variant(value, VariantType::Enum);
		}
		case VariantType::Vec2: {
			Vec2 value {};
			if (!backend.try_read("x", value.x) || !backend.try_read("y", value.y))
				return std::nullopt;
			return Variant(value);
		}
		case VariantType::Vec3: {
			Vec3 value {};
			if (!backend.try_read("value", value))
				return std::nullopt;
			return Variant(value);
		}
		case VariantType::Vec4: {
			glm::vec4 value {};
			if (!backend.try_read("x", value.x) || !backend.try_read("y", value.y) || !backend.try_read("z", value.z) || !backend.try_read("w", value.w))
				return std::nullopt;
			return Variant(value);
		}
		case VariantType::Entity: {
			std::string value;
			if (!backend.try_read("value", value))
				return std::nullopt;
			return Variant(RID::from_string(value));
		}
		case VariantType::Asset: {
			std::string value;
			if (!backend.try_read("value", value))
				return std::nullopt;
			return Variant(UUID::from_string(value));
		}
		case VariantType::Struct:
			return std::nullopt;
		}
		return std::nullopt;
	}
}

ComponentDataManager::ComponentDataManager() {
	subscribe_to_registry_updates();
}

ComponentDataManager::ComponentDataManager(const ComponentDataManager& other)
    : m_data(other.m_data) {
	subscribe_to_registry_updates();
}

ComponentDataManager& ComponentDataManager::operator=(const ComponentDataManager& other) {
	if (this == &other)
		return *this;

	m_data = other.m_data;
	if (!m_has_component_updated_listener)
		subscribe_to_registry_updates();

	return *this;
}

ComponentDataManager::~ComponentDataManager() {
	unsubscribe_from_registry_updates();
}

void ComponentDataManager::subscribe_to_registry_updates() {
	if (!m_has_component_updated_listener) {
		m_component_updated_listener = ComponentRegistry::on_component_updated().add_listener([&](ComponentTypeID updated) {
			std::vector<RID> affected;
			for (const auto& [entity, components] : m_data) {
				for (const auto& component : components) {
					if (component.ID == updated) {
						affected.push_back(entity);
						break;
					}
				}
			}
			for (RID entity : affected)
				migrate_component(entity, updated);
		});
		m_has_component_updated_listener = true;
	}

	if (!m_has_component_removed_listener) {
		m_component_removed_listener = ComponentRegistry::on_component_removed().add_listener([&](ComponentTypeID removed) {
			for (auto it = m_data.begin(); it != m_data.end();) {
				auto& components = it->second;
				std::erase_if(components, [removed](const ComponentInstanceData& component) {
					return component.ID == removed;
				});

				if (components.empty())
					it = m_data.erase(it);
				else
					++it;
			}
		});
		m_has_component_removed_listener = true;
	}
}

void ComponentDataManager::unsubscribe_from_registry_updates() {
	if (m_has_component_updated_listener) {
		ComponentRegistry::on_component_updated().remove_listener(m_component_updated_listener);
		m_has_component_updated_listener = false;
	}
	if (m_has_component_removed_listener) {
		ComponentRegistry::on_component_removed().remove_listener(m_component_removed_listener);
		m_has_component_removed_listener = false;
	}
}

void ComponentDataManager::add(RID entity, ComponentTypeID type) {
	auto& components = m_data[entity];
	for (const auto& component : components) {
		if (component.ID == type)
			return;
	}

	const auto& desc = ComponentRegistry::get(type);
	ComponentInstanceData instance {};
	instance.ID = type;
	instance.Fields.reserve(desc.Fields.size());
	for (const auto& field : desc.Fields)
		instance.Fields.push_back(make_reflected_field_value(field));

	components.push_back(std::move(instance));
}

ComponentInstanceData* ComponentDataManager::get(RID entity, ComponentTypeID type) {
	auto it = m_data.find(entity);
	if (it == m_data.end())
		return nullptr;

	for (auto& component : it->second) {
		if (component.ID == type)
			return &component;
	}
	return nullptr;
}

const ComponentInstanceData* ComponentDataManager::get(RID entity, ComponentTypeID type) const {
	auto it = m_data.find(entity);
	if (it == m_data.end())
		return nullptr;

	for (const auto& component : it->second) {
		if (component.ID == type)
			return &component;
	}
	return nullptr;
}

const ReflectedFieldValue* ComponentDataManager::find_field(RID entity, std::string_view name) const {
	auto it = m_data.find(entity);
	if (it == m_data.end())
		return nullptr;

	for (const auto& component : it->second) {
		for (const auto& field : component.Fields) {
			if (field.Name == name)
				return &field;
		}
	}

	return nullptr;
}

bool ComponentDataManager::has(RID entity, ComponentTypeID type) const {
	return get(entity, type) != nullptr;
}

void ComponentDataManager::copy_components(RID source, RID target) {
	auto it = m_data.find(source);
	if (it == m_data.end())
		return;

	m_data[target] = it->second;
}

void ComponentDataManager::serialize_entity(RID entity, ISerializer& backend) const {
	backend.begin_array("components");

	auto it = m_data.find(entity);
	if (it != m_data.end()) {
		for (const auto& component : it->second) {
			backend.begin_array_object();
			backend.write("type_id", component.ID.to_string());

			if (ComponentRegistry::exists(component.ID)) {
				const auto& descriptor = ComponentRegistry::get(component.ID);
				backend.write("name", descriptor.Name);
				backend.write("asset_id", descriptor.AssetID.to_string());
				backend.write("version", static_cast<int>(descriptor.Version));
			}

			backend.begin_array("fields");
			for (const auto& field : component.Fields) {
				backend.begin_array_object();
				backend.write("field_id", field.ID.to_string());
				backend.write("name", field.Name);
				backend.write("type", variant_type_to_string(field.Value.get_type()));
				serialize_variant_value(field.Value, backend);
				backend.end_array_object();
			}
			backend.end_array();
			backend.end_array_object();
		}
	}

	backend.end_array();
}

void ComponentDataManager::deserialize_entity(RID entity, ISerializer& backend) {
	backend.begin_array("components");

	while (backend.try_begin_array_object_read()) {
		std::string type_id_str;
		std::string name;
		std::string asset_id_str;
		backend.try_read("type_id", type_id_str);
		backend.try_read("name", name);
		backend.try_read("asset_id", asset_id_str);

		std::optional<ComponentTypeID> component_id;
		if (!type_id_str.empty()) {
			try {
				ComponentTypeID parsed = ComponentTypeID::from_string(type_id_str);
				if (ComponentRegistry::exists(parsed))
					component_id = parsed;
			} catch (const std::exception&) {
			}
		}
		if (!component_id.has_value() && !asset_id_str.empty() && !name.empty()) {
			try {
				component_id = ComponentRegistry::try_find_by_asset_and_name(UUID::from_string(asset_id_str), name);
			} catch (const std::exception&) {
			}
		}
		if (!component_id.has_value() && !name.empty())
			component_id = ComponentRegistry::try_find_by_name(name);

		if (!component_id.has_value()) {
			backend.end_array_object();
			continue;
		}

		add(entity, *component_id);
		auto* instance = get(entity, *component_id);

		backend.begin_array("fields");
		while (backend.try_begin_array_object_read()) {
			std::string field_id_str;
			std::string field_name;
			std::string field_type_str;
			backend.try_read("field_id", field_id_str);
			backend.try_read("name", field_name);
			backend.try_read("type", field_type_str);

			const auto field_type = variant_type_from_string(field_type_str);
			const auto value = field_type ? deserialize_variant_value(*field_type, backend) : std::nullopt;
			if (instance && value.has_value()) {
				auto field_it = std::find_if(instance->Fields.begin(), instance->Fields.end(), [&](const ReflectedFieldValue& field) {
					if (!field_id_str.empty()) {
						try {
							return field.ID == FieldID::from_string(field_id_str);
						} catch (const std::exception&) {
						}
					}
					return field.Name == field_name;
				});

				if (field_it != instance->Fields.end() && field_it->Value.get_type() == *field_type)
					field_it->Value = *value;
			}
			backend.end_array_object();
		}
		backend.end_array();
		backend.end_array_object();
	}

	backend.end_array();
}

void ComponentDataManager::remove(RID entity, ComponentTypeID type) {
	remove_if_exists(entity, type);
}

bool ComponentDataManager::remove_if_exists(RID entity, ComponentTypeID type) {
	auto it = m_data.find(entity);
	if (it == m_data.end())
		return false;

	auto& components = it->second;
	const std::size_t old_size = components.size();
	std::erase_if(components, [type](const ComponentInstanceData& component) {
		return component.ID == type;
	});

	const bool removed = components.size() != old_size;
	if (components.empty())
		m_data.erase(it);

	return removed;
}

void ComponentDataManager::remove_all_components(RID entity) {
	m_data.erase(entity);
}

void ComponentDataManager::migrate_component(RID entity, ComponentTypeID type) {
	auto* instance = get(entity, type);
	if (!instance)
		return;

	const auto& desc = ComponentRegistry::get(type);
	ReflectedFieldStorage new_fields;
	new_fields.reserve(desc.Fields.size());

	for (const auto& new_field : desc.Fields) {
		auto it = std::find_if(instance->Fields.begin(), instance->Fields.end(), [&](const ReflectedFieldValue& old_field) {
			return old_field.ID == new_field.ID;
		});

		if (it != instance->Fields.end() && it->Value.get_type() == new_field.DefaultValue.get_type()) {
			new_fields.push_back(*it);
			new_fields.back().Name = new_field.Name;
			new_fields.back().Type = new_field.Type;
		} else {
			new_fields.push_back(make_reflected_field_value(new_field));
		}
	}

	instance->Fields = std::move(new_fields);
}

}
