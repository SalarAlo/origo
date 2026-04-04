#pragma once

#include "origo/scene/Scene.h"

namespace Origo {

class SceneEntitySerializationManager {
public:
	void serialize_entities(const Scene& scene, ISerializer& backend) const;
	void deserialize_entities(Scene& scene, ISerializer& backend) const;

	void serialize_entity_data(const Scene& scene, RID entity, ISerializer& backend) const;
	void serialize_entity(const Scene& scene, RID entity, ISerializer& backend) const;
	void deserialize_entity(Scene& scene, ISerializer& backend, int entity_index) const;
	void deserialize_entity_components(Scene& scene, RID entity, ISerializer& backend) const;
	void deserialize_native_components(Scene& scene, RID entity, ISerializer& backend) const;
};

}
