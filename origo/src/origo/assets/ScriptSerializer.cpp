#include "origo/assets/ScriptSerializer.h"
#include "origo/assets/Script.h"

namespace Origo {

void ScriptSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type shader");
	auto script { static_cast<const Script*>(asset) };
	backend.Write("path", script->GetPath().c_str());
	backend.Write("id", script->GetID().ToString());
}

void ScriptSerializer::Deserialize(ISerializer& backend, Asset& asset) const {
	std::string path;
	std::string id;

	if (!backend.TryRead("id", id)) {
		ORG_CORE_ERROR("Unable to deserialize Script Asset because backend has no id entry");
		return;
	}

	if (!backend.TryRead("path", path)) {
		ORG_CORE_ERROR("Unable to deserialize Script Asset because backend has no source entry");
		return;
	}

	auto& script { static_cast<Script&>(asset) };
	script.SetPath(path);
	script.SetID(UUID::FromString(id));
}

}
