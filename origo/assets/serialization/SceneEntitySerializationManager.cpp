#include "origo/assets/serialization/SceneEntitySerializationManager.h"

#include "origo/components/NativeComponentRegistry.h"

namespace Origo {

void SceneEntitySerializationManager::serialize_entities(const Scene& scene, ISerializer& backend) const {
	backend.begin_array("entities");

	for (RID entity : scene.get_entities()) {
		serialize_entity(scene, entity, backend);
	}

	backend.end_array();
}

void SceneEntitySerializationManager::serialize_entity(const Scene& scene, RID entity, ISerializer& backend) const {
	backend.begin_array_object();
	serialize_entity_data(scene, entity, backend);
	backend.end_array_object();
}

void SceneEntitySerializationManager::serialize_entity_data(const Scene& scene, RID entity, ISerializer& backend) const {
	backend.write("rid", entity.to_string());

	scene.m_native_component_manager.serialize_entity(entity, backend);
	scene.m_script_component_manager.serialize_entity(entity, backend);
}

void SceneEntitySerializationManager::deserialize_entities(Scene& scene, ISerializer& backend) const {
	backend.begin_array("entities");

	int entity_index = 0;
	while (backend.try_begin_array_object_read()) {
		deserialize_entity(scene, backend, entity_index);
		++entity_index;
	}

	backend.end_array();
}

void SceneEntitySerializationManager::deserialize_entity(Scene& scene, ISerializer& backend, int entity_index) const {
	std::string rid_str;

	if (!backend.try_read("rid", rid_str)) {
		ORG_WARN("[Scene] Entity {} has no rid", entity_index);
		backend.end_array_object();
		return;
	}

	RID entity = scene.create_entity_with_rid(RID::from_string(rid_str));
	deserialize_entity_components(scene, entity, backend);

	backend.end_array_object();
}

void SceneEntitySerializationManager::deserialize_entity_components(Scene& scene, RID entity, ISerializer& backend) const {
	deserialize_native_components(scene, entity, backend);
	scene.m_script_component_manager.deserialize_entity(entity, backend);
}

void SceneEntitySerializationManager::deserialize_native_components(Scene& scene, RID entity, ISerializer& backend) const {
	backend.begin_array("native_components");

	int comp_index = 0;
	while (backend.try_begin_array_object_read()) {
		std::string type_name;
		if (!backend.try_read("type", type_name)) {
			ORG_WARN("[Scene]  Component {} has no type", comp_index);
			backend.end_array_object();
			++comp_index;
			continue;
		}

		const auto* info = NativeComponentRegistry::get_instance().get_component_info_by_name(type_name);

		if (!info) {
			ORG_WARN("[Scene]  Unknown component '{}'", type_name);
			backend.end_array_object();
			++comp_index;
			continue;
		}
		if (!info->Serializer) {
			ORG_WARN("[Scene]  Component '{}' has no serializer", type_name);
			backend.end_array_object();
			++comp_index;
			continue;
		}

		info->Add(scene.m_native_component_manager, entity);

		void* component = info->Get(scene.m_native_component_manager, entity);

		backend.begin_object("data");
		info->Serializer->deserialize(static_cast<Component*>(component), backend);
		backend.end_object();

		backend.end_array_object();
		++comp_index;
	}

	backend.end_array();
}

}
