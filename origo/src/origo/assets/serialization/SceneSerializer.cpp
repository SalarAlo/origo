#include "SceneSerializer.h"
#include "origo/components/NativeComponentRegistry.h"
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

Scope<Scene> DeserializeFromFile(const std::filesystem::path& path) {
	JsonSerializer backend(path);
	backend.LoadFile();

	std::string sceneName = "Unnamed Scene";
	backend.TryRead("name", sceneName);

	auto scene = std::make_unique<Scene>(sceneName);

	backend.BeginArray("entities");

	int entityIndex = 0;

	while (backend.TryBeginArrayElementRead()) {
		std::string ridStr;
		if (!backend.TryRead("rid", ridStr)) {
			ORG_WARN("[Scene] Entity {} has no rid", entityIndex);
			backend.EndArrayElement();
			++entityIndex;
			continue;
		}

		RID entity = scene->CreateEntity(ridStr);

		backend.BeginArray("native_components");

		int compIndex = 0;

		while (backend.TryBeginArrayElementRead()) {

			std::string typeName;
			if (!backend.TryRead("type", typeName)) {
				ORG_WARN("[Scene]  Component {} has no type", compIndex);
				backend.EndArrayElement();
				++compIndex;
				continue;
			}

			const auto* info = NativeComponentRegistry::GetByName(typeName);
			if (!info) {
				ORG_WARN("[Scene]  Unknown component '{}'", typeName);
				backend.EndArrayElement();
				++compIndex;
				continue;
			}

			info->Add(scene->m_NativeComponentManager, entity);

			void* component = info->Get(scene->m_NativeComponentManager, entity);

			backend.BeginObject("data");

			info->Serializer->Deserialize(static_cast<Component*>(component), backend);

			backend.EndObject();

			backend.EndArrayElement();

			++compIndex;
		}

		backend.EndArray();
		backend.EndArrayElement();

		++entityIndex;
	}

	backend.EndArray();

	return scene;
}
}
