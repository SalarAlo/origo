#pragma once

#include <string>
#include <vector>

#include "origo/scripting/FieldDescriptor.h"

namespace Origo {

using ScriptTypeID = TypeID;

struct ScriptLifecycleBindings {
	RuntimeFunctionHandle OnStart { UUID::bad() };
	std::string OnStartBody {};
	RuntimeFunctionHandle OnUpdate { UUID::bad() };
	std::string OnUpdateBody {};
	RuntimeFunctionHandle OnFixedUpdate { UUID::bad() };
	std::string OnFixedUpdateBody {};
	RuntimeFunctionHandle OnDestroy { UUID::bad() };
	std::string OnDestroyBody {};
	RuntimeFunctionHandle OnCollision { UUID::bad() };
	std::string OnCollisionBody {};
	RuntimeFunctionHandle OnTriggerEnter { UUID::bad() };
	std::string OnTriggerEnterBody {};
	RuntimeFunctionHandle OnTriggerExit { UUID::bad() };
	std::string OnTriggerExitBody {};
};

struct ScriptDescriptor {
	ScriptTypeID ID { UUID::bad() };
	UUID AssetID { UUID::bad() };
	std::string Name {};
	TypeRef ComponentType {};
	uint32_t Version {};
	RuntimeTypeHandle RuntimeType { UUID::bad() };
	std::vector<FieldDescriptor> Fields {};
	std::vector<AnnotationMetadata> Annotations {};
	ScriptLifecycleBindings Lifecycle {};
};

}
