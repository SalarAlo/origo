#pragma once

#include "origo/renderer/Shader.h"

namespace Origo {
enum class TextureType {
	Diffuse,
};

class Texture {
public:
	Texture(std::string_view path, TextureType type = TextureType::Diffuse);
	void Bind(Ref<Shader>);

private:
	GLuint m_TextureId;
	TextureType m_Type;
};

}
