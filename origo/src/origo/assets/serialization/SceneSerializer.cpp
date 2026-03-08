#include "SceneSerializer.h"

#include "origo/components/NativeComponentRegistry.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo::SceneSerializer {

void serialize_to_file(Scene& scene, const std::filesystem::path& out) {
	JsonSerializer backend(out);

	backend.write("name", scene.get_name());

	backend.begin_array("entities");

	for (RID e : scene.get_entities()) {
		backend.begin_array_object();

		backend.write("rid", e.to_string());

		scene.m_native_component_manager.serialize_entity(e, backend);

		backend.end_array_object();
	}

	backend.end_array();

	backend.save_to_file();
}

Scope<Scene> deserialize_from_file(const std::filesystem::path& path) {
	JsonSerializer backend(path);
	backend.load_file();

	std::string scene_name = "Unnamed Scene";
	backend.try_read("name", scene_name);

	auto scene = std::make_unique<Scene>(scene_name);

	backend.begin_array("entities");

	int entity_index = 0;

	while (backend.try_begin_array_object_read()) {
		std::string rid_str;
		if (!backend.try_read("rid", rid_str)) {
			ORG_WARN("[Scene] Entity {} has no rid", entity_index);
			backend.end_array_object();
			++entity_index;
			continue;
		}

		RID entity = scene->create_entity_with_rid(RID::from_string(rid_str));

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

			info->Add(scene->m_native_component_manager, entity);

			void* component = info->Get(scene->m_native_component_manager, entity);

			backend.begin_object("data");

			info->Serializer->deserialize(static_cast<Component*>(component), backend);

			backend.end_object();

			backend.end_array_object();

			++comp_index;
		}

		backend.end_array();
		backend.end_array_object();

		++entity_index;
	}

	backend.end_array();

	return scene;
}
}
