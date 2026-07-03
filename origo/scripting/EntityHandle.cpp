#include "origo/scripting/EntityHandle.h"

#include "origo/components/TransformComponent.h"
#include "origo/scene/Scene.h"

namespace Origo {

bool EntityHandle::is_valid() const {
	return OwnerScene && OwnerScene->has_entity(Entity);
}

void EntityHandle::translate(float x, float y, float z) const {
	if (!is_valid())
		return;

	auto* transform = OwnerScene->get_native_component<TransformComponent>(Entity);
	if (!transform)
		return;

	transform->translate(Vec3 { x, y, z });
}

}
