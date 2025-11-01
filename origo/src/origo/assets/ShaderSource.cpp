#include "origo/assets/ShaderSource.h"
#include "nlohmann/json_fwd.hpp"

#pragma region IO_RELATED
static std::string ReadFile(std::string_view path) {
	constexpr std::size_t readSize { 4096 };
	auto stream = std::ifstream(path.data());

	if (!stream) {
		ORG_CORE_ERROR("[Shader] Trying to read non existent file!");
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

static Origo::ShaderData GetData(std::string_view name) {
	Origo::ShaderData data {};
	std::string fileContent { ReadFile("./resources/shaders/" + std::string(name) + ".glsl") };

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

}
