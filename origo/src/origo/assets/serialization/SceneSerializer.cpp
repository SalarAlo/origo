#include "SceneSerializer.h"

#include "origo/assets/serialization/SceneEntitySerializationManager.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo::SceneSerializer {

void serialize_to_file(Scene& scene, const std::filesystem::path& out) {
	JsonSerializer backend(out);
	SceneEntitySerializationManager entity_serializer {};

	backend.write("name", scene.get_name());
	entity_serializer.serialize_entities(scene, backend);

	backend.save_to_file();
}

Scope<Scene> deserialize_from_file(const std::filesystem::path& path) {
	JsonSerializer backend(path);
	SceneEntitySerializationManager entity_serializer {};
	backend.load_file();

	std::string scene_name = "Unnamed Scene";
	backend.try_read("name", scene_name);

	auto scene = std::make_unique<Scene>(scene_name);
	entity_serializer.deserialize_entities(*scene, backend);

	return scene;
}
}
