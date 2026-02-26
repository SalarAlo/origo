#include "origo/assets/serialization/ScriptSerializer.h"

#include "origo/assets/Script.h"

namespace Origo {

void ScriptSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type shader");
	auto script { static_cast<const Script*>(asset) };
	backend.write("path", script->get_path().c_str());
	backend.write("id", script->get_id().to_string());
}

void ScriptSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	std::string path;
	std::string id;

	if (!backend.try_read("id", id)) {
		ORG_CORE_ERROR("Unable to deserialize Script Asset because backend has no id entry");
		return;
	}

	if (!backend.try_read("path", path)) {
		ORG_CORE_ERROR("Unable to deserialize Script Asset because backend has no source entry");
		return;
	}

	auto& script { static_cast<Script&>(asset) };
	script.set_path(path);
	script.set_id(UUID::from_string(id));
}

}
