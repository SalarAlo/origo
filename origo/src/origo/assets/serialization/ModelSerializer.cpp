#include "origo/assets/serialization/ModelSerializer.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Model.h"
#include "origo/core/Logger.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void ModelSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type model");

	auto model { static_cast<const Model*>(asset) };

	if (auto shaderHandle { model->GetShaderHandle() }; shaderHandle.has_value()) {
		auto shaderID { AssetManager::GetInstance().GetUUID(*shaderHandle) };
		if (shaderID.has_value())
			backend.Write("shader_id", (*shaderID).ToString());
	} else {
		backend.Write("shader_id", "0");
	}

	auto path { model->GetPath() };
	backend.Write("path", path.c_str());
}

void ModelSerializer::Deserialize(ISerializer& backend, Asset& asset) const {
	std::string path {};
	std::string shaderID {};

	backend.TryRead("shader_id", shaderID);

	if (!backend.TryRead("path", path)) {
		ORG_CORE_WARN("No Path provided");
	} else {
		ORG_CORE_TRACE("Path {} provided", path);
	}

	auto& model { static_cast<Model&>(asset) };

	model.SetPath(path);
	model.SetShaderUUID(UUID::FromString(shaderID));
}

}
