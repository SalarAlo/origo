#pragma once

#include "origo/core/RID.h"

namespace Origo {
enum class SceneCommandType {
	Removal,
};

struct SceneCommand {
	SceneCommand(SceneCommandType type)
	    : Type(type) { }
	SceneCommandType Type;
};

struct RemoveEntityCommand : SceneCommand {
	RemoveEntityCommand(const RID& rid)
	    : SceneCommand(SceneCommandType::Removal)
	    , EntityToRemove(rid) { };

	RID EntityToRemove;
};

}
