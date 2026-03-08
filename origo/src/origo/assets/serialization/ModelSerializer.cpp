#include "origo/assets/serialization/ModelSerializer.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/model/Model.h"

#include "origo/core/Logger.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void ModelSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	ORG_CORE_TRACE("Seriliazing an asset of type model");

	auto model { static_cast<const Model*>(asset) };

	backend.write("shader_id", model->get_shader_handle());

	auto path { model->get_path() };
	backend.write("path", path.c_str());
}

void ModelSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	std::string path {};
	std::string shader_id {};

	backend.try_read("shader_id", shader_id);

	if (!backend.try_read("path", path)) {
		ORG_CORE_WARN("No Path provided");
	} else {
		ORG_CORE_TRACE("Path {} provided", path);
	}

	auto& model { static_cast<Model&>(asset) };

	model.set_path(path);
	model.set_shader_uuid(UUID::from_string(shader_id));
}

}
