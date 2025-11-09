#include "origo/assets/ShaderSourceSerializer.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/ShaderSource.h"

namespace Origo::ShaderSourceSerializer {

void Serialize(ShaderSource* src, ISerializer& backend) {
	backend.BeginObject("source");
	backend.Write("type", src->GetTypeName());

	if (src->GetTypeName() == ShaderSourceRaw::GetTypeNameClass()) {
		auto data { src->GetShaderData() };
		backend.Write("vertex_shader", data.VertexShader);
		backend.Write("fragment_shader", data.FragmentShader);
	} else if (src->GetTypeName() == ShaderSourceFile::GetTypeNameClass()) {
		auto srcFile { dynamic_cast<ShaderSourceFile*>(src) };
		backend.Write("path", srcFile->GetPath());
	}

	backend.EndObject();
}

ShaderSource* Deserialize(const nlohmann::json& j) {
	return nullptr;
}

}
