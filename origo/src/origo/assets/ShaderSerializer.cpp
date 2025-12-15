
#include "origo/assets/ShaderSerializer.h"
#include "origo/assets/Shader.h"

namespace Origo {

void ShaderSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type shader");
	auto shader { static_cast<const Shader*>(asset) };
	auto source { shader->GetSource() };
	source->Serialize(backend);
}

void ShaderSerializer::Deserialize(ISerializer& backend, Asset& asset) const {
	auto source { ShaderSource::Deserialize(backend) };
	auto& shader { static_cast<Shader&>(asset) };
	shader.SetSource(std::move(source));
}

}
