#include "origo/assets/TextureSource.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "stb_image.h"

#include "origo/assets/Texture.h"
#include "origo/renderer/GlDebug.h"

#include "origo/core/Logger.h"

namespace Origo {

Texture::Texture(const std::string& path, TextureType type)
    : m_TextureType(type)
    , m_Source(MakeScope<TextureSourceFile>(path)) {
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	if (!data) {
		ORG_ERROR("[Texture] Failed to load Texture with path {}", path.data());
	}

	InitTexture(width, height, channels, data);

	stbi_image_free(data);
}

Texture::Texture(const aiTexture* embeddedTex, TextureType type)
    : m_TextureType(type)
    , m_Source(MakeScope<TextureSourceEmbedded>()) {

	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* decoded = nullptr;

	if (embeddedTex->mHeight == 0) {
		// Compressed image (PNG, JPEG, etc.)
		decoded = stbi_load_from_memory(
		    reinterpret_cast<const unsigned char*>(embeddedTex->pcData),
		    embeddedTex->mWidth,
		    &width,
		    &height,
		    &channels,
		    0);
	} else {
		// Uncompressed RGBA data
		width = embeddedTex->mWidth;
		height = embeddedTex->mHeight;
		channels = 4; // aiTexel is RGBA
		decoded = reinterpret_cast<unsigned char*>(embeddedTex->pcData);
	}

	if (!decoded) {
		ORG_ERROR("[Texture] Failed to load embedded texture.");
		return;
	}

	InitTexture(width, height, channels, decoded);

	if (embeddedTex->mHeight == 0)
		stbi_image_free(decoded);
}

void Texture::InitTexture(int width, int height, int channels, unsigned char* data) {
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
}

void Texture::Bind(Shader* shader) const {
	int slot { static_cast<int>(m_TextureType) };
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	std::string typeName { magic_enum::enum_name(m_TextureType) };
	shader->SetUniform(
	    "u_Texture_" + typeName,
	    slot);
}

};
