
#include "origo/assets/ShaderSerializer.h"
#include "origo/assets/ShaderSourceSerializer.h"
#include "origo/assets/Shader.h"

namespace Origo {

nlohmann::json ShaderSerializer::Serialize(const Ref<Asset>& asset) const {
	Ref<Shader> shader = std::dynamic_pointer_cast<Shader>(asset);
	nlohmann::json shaderSerialized;
	nlohmann::json source = ShaderSourceSerializer::Serialize(shader->GetSource());

	shaderSerialized["source"] = nlohmann::json(source);

	return shaderSerialized;
}

Ref<Asset> ShaderSerializer::Deserialize(const nlohmann::json& j) const {
	return nullptr;
}

}
