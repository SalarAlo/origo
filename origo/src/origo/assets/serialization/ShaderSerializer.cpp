#include "origo/assets/serialization/ShaderSerializer.h"

#include "origo/assets/Shader.h"

namespace Origo {

void ShaderSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type shader");
	auto shader { static_cast<const Shader*>(asset) };
	auto source { shader->get_source() };
	source->serialize(backend);
}

void ShaderSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	auto source { ShaderSource::deserialize(backend) };
	auto& shader { static_cast<Shader&>(asset) };
	shader.set_source(std::move(source));
}

}
