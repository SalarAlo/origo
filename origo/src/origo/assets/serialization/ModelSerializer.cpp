#include "origo/assets/serialization/ModelSerializer.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/Model.h"

#include "origo/core/Logger.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void ModelSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type model");

	auto model { static_cast<const Model*>(asset) };

	if (auto shader_handle { model->get_shader_handle() }; shader_handle.has_value()) {
		auto shader_id { AssetManager::get_instance().get_uuid(*shader_handle) };
		if (shader_id.has_value())
			backend.write("shader_id", (*shader_id).to_string());
	} else {
		backend.write("shader_id", "0");
	}

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
