#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "stb_image.h"

#include "origo/assets/Texture.h"
#include "origo/renderer/GlDebug.h"

#include "origo/core/Logger.h"

namespace Origo {

int TextureTypeToSlot(TextureType type) {
	switch (type) {
	case TextureType::Albedo:
		return 0;
	default:
		return -1;
	}
}

Texture::Texture(const std::string& path, TextureType type)
    : m_TextureType(type)
    , m_Path(path) {
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* data = stbi_load(("resources/textures/" + path).c_str(), &width, &height, &channels, 0);

	if (!data) {
		ORG_ERROR("[Texture] Failed to load Texture with path {}", path.data());
	}
	GLCall(glGenTextures(1, &m_TextureId));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	GLCall(glTexImage2D(
	    GL_TEXTURE_2D,
	    0, // mipmap level
	    (channels == 4 ? GL_RGBA : GL_RGB),
	    width,
	    height,
	    0,
	    (channels == 4 ? GL_RGBA : GL_RGB),
	    GL_UNSIGNED_BYTE,
	    data));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	stbi_image_free(data);
}

void Texture::Bind(Ref<Shader> shader) const {
	int slot { TextureTypeToSlot(m_TextureType) };
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	std::string typeName { magic_enum::enum_name(m_TextureType) };
	shader->SetUniform(
	    "u_Texture_" + typeName,
	    slot);
}

TextureType Texture::GetType() const {
	return m_TextureType;
}

};
