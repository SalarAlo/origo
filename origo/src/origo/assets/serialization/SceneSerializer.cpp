#include "SceneSerializer.h"
#include "origo/serialization/JsonSerializer.h"

namespace Origo::SceneSerializer {

void SerializeToFile(Scene& scene, const std::filesystem::path& out) {
	JsonSerializer backend(out);

	backend.Write("name", scene.GetName());

	backend.BeginArray("entities");
	for (RID e : scene.GetEntities()) {
		backend.BeginArrayElement();

		backend.Write("rid", e.ToString());

		scene.m_NativeComponentManager.SerializeEntity(e, backend);

		backend.EndArrayElement();
	}
	backend.EndArray();

	backend.SaveToFile();
}
}
