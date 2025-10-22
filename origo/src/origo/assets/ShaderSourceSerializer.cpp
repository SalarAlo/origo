#include "origo/assets/ShaderSourceSerializer.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/ShaderSource.h"
#include <memory>

namespace Origo::ShaderSourceSerializer {

nlohmann::json Serialize(Ref<ShaderSource> src) {
	nlohmann::json json;
	json["type"] = src->GetTypeName();

	if (src->GetTypeName() == ShaderSourceRaw::GetTypeNameClass()) {
		auto data { src->GetShaderData() };
		json["vertex_shader"] = data.VertexShader;
		json["fragment_shader"] = data.FragmentShader;
	} else if (src->GetTypeName() == ShaderSourceFile::GetTypeNameClass()) {
		auto srcFile { std::dynamic_pointer_cast<ShaderSourceFile>(src) };
		json["path"] = srcFile->GetPath();
	}

	return json;
}

Ref<ShaderSource> Deserialize(const nlohmann::json& j) {
	auto type { j["type"] };
	if (type == ShaderSourceRaw::GetTypeNameClass()) {
		std::string vertexShader = j["vertex_shader"];
		std::string fragmentShader = j["fragment_shader"];
		return MakeRef<ShaderSourceRaw>(vertexShader, fragmentShader);
	}

	if (type == ShaderSourceFile::GetTypeNameClass()) {
		std::string path = j["path"];
		return MakeRef<ShaderSourceFile>(path);
	}

	return nullptr;
}

}
