
#include "origo/assets/ShaderSerializer.h"
#include "origo/assets/Shader.h"

// TODO

namespace Origo {

void ShaderSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type shader");
	const Shader* shader = static_cast<const Shader*>(asset);
	auto source { shader->GetSource() };
	source->Serialize(backend);
}

Scope<Asset> ShaderSerializer::Deserialize(ISerializer& backend) const {
	throw std::logic_error("Not implemented");
}

}
