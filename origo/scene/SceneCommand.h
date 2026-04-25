#pragma once

#include "origo/core/RID.h"

namespace Origo {
enum class SceneCommandType {
	RemoveEntity,
	RemoveEntityNativeComponent,
};

struct SceneCommand {
	SceneCommand(SceneCommandType type)
	    : Type(type) { }
	SceneCommandType Type;
};

struct RemoveEntityCommand : SceneCommand {
	RemoveEntityCommand(const RID& rid)
	    : SceneCommand(SceneCommandType::RemoveEntity)
	    , EntityToRemove(rid) { };

	RID EntityToRemove;
};

struct RemoveEntityNativeComponentCommand : SceneCommand {
	RemoveEntityNativeComponentCommand(const RID& rid, const std::type_index& idx)
	    : SceneCommand(SceneCommandType::RemoveEntityNativeComponent)
	    , ComponentToRemove(idx)
	    , EntityToRemoveComponent(rid) { };

	RID EntityToRemoveComponent;
	std::type_index ComponentToRemove;
};

}
