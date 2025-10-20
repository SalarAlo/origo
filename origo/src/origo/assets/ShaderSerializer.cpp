
#pragma once

#include "origo/assets/ShaderSerializer.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/ShaderSourceSerializer.h"
#include "origo/assets/Shader.h"

namespace Origo {

nlohmann::json ShaderSerializer::Serialize(const Ref<Asset>& asset) const {
	Ref<Shader> shader = std::dynamic_pointer_cast<Shader>(asset);
	return ShaderSourceSerializer::Serialize(shader->GetSource());
}

Ref<Asset> ShaderSerializer::Deserialize(const nlohmann::json& j) const {
	return nullptr;
}

REGISTER_ASSET_SERIALIZER(Shader)
}
