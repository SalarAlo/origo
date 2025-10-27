#include "origo/assets/ShaderSourceSerializer.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/ShaderSource.h"
#include <memory>

namespace Origo::ShaderSourceSerializer {

void Serialize(Ref<ShaderSource> src, ISerializer& backend) {
	backend.BeginObject("source");
	backend.Write("type", src->GetTypeName());

	if (src->GetTypeName() == ShaderSourceRaw::GetTypeNameClass()) {
		auto data { src->GetShaderData() };
		backend.Write("vertex_shader", data.VertexShader);
		backend.Write("fragment_shader", data.FragmentShader);
	} else if (src->GetTypeName() == ShaderSourceFile::GetTypeNameClass()) {
		auto srcFile { std::dynamic_pointer_cast<ShaderSourceFile>(src) };
		backend.Write("path", srcFile->GetPath());
	}

	backend.EndObject();
}

Ref<ShaderSource> Deserialize(const nlohmann::json& j) {
	return nullptr;
}

}
