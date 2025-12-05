#include "origo/assets/ShaderSource.h"
#include "origo/core/Logger.h"

#pragma region IO_RELATED
static std::string ReadFile(std::string_view path) {
	constexpr std::size_t readSize { 4096 };
	auto stream = std::ifstream(path.data());

	if (!stream) {
		ORG_CORE_ERROR("[Shader] Trying to read non existent file! ({})", path);
		throw std::ios_base::failure("file does not exist");
	}

	std::string out {};
	std::string buf(readSize, '\0');
	while (stream.read(&buf[0], readSize)) {
		out.append(buf, 0, stream.gcount());
	}
	out.append(buf, 0, stream.gcount());
	return out;
}

static Origo::ShaderData GetData(std::string_view path) {
	Origo::ShaderData data {};
	std::string fileContent { ReadFile(path) };

	auto vertPos { fileContent.find("#VERTEX") };
	auto fragPos { fileContent.find("#FRAGMENT") };

	if (vertPos == std::string::npos || fragPos == std::string::npos) {
		ORG_CORE_ERROR("[Shader] file missing #VERTEX or #FRAGMENT section");
		throw std::runtime_error("Shader file missing #VERTEX or #FRAGMENT section");
	}

	std::string vertexSrc { fileContent.substr(vertPos + 7, fragPos - (vertPos + 7)) };
	std::string fragmentSrc { fileContent.substr(fragPos + 9) };

	static std::string vertexStorage, fragmentStorage;
	vertexStorage = std::move(vertexSrc);
	fragmentStorage = std::move(fragmentSrc);

	data.VertexShader = vertexStorage.c_str();
	data.FragmentShader = fragmentStorage.c_str();

	return data;
}
#pragma endregion

namespace Origo {

ShaderData ShaderSourceFile::GetShaderData() const {
	return GetData(m_Path);
}

void ShaderSource::Serialize(ISerializer& backend) const {
	backend.BeginObject("source");
	backend.Write("type", magic_enum::enum_name(GetSourceType()));
	SerializeBody(backend);
	backend.EndObject();
}

Scope<ShaderSource> ShaderSource::Deserialize(ISerializer& backend) {
	backend.BeginObject("source");

	std::string typeStr {};
	backend.TryRead("type", typeStr);
	auto typeOptional { magic_enum::enum_cast<ShaderSourceType>(typeStr) };
	if (!typeOptional.has_value()) {
		ORG_ERROR("ShaderSource::Deserialize: Unidentifiable type {}", typeStr);
		return nullptr;
	}
	auto type { typeOptional.value() };

	switch (type) {
	case ShaderSourceType::File: {
		std::string path {};
		backend.TryRead("shader_path", path);
		backend.EndObject();
		return MakeScope<ShaderSourceFile>(path);
	}

	case ShaderSourceType::Raw: {
		std::string vert {};
		std::string frag {};
		backend.TryRead("vertex_shader", vert);
		backend.TryRead("fragment_shader", frag);
		backend.EndObject();
		return MakeScope<ShaderSourceRaw>(vert, frag);
	}
	default: {
		backend.EndObject();
		ORG_ERROR("ShaderSource::Deserialize: Unsupported type {}", typeStr);
		return nullptr;
	}
	}
}

}
