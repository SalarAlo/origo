
#define STB_IMAGE_IMPLEMENTATION

#include "engine/Texture.h"
#include <stb_image.h>

Texture::Texture(std::string_view pathName) {
	auto fullPath { std::string("./resources/textures/") + std::string(pathName) };
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(fullPath.data(), &m_ImageWidth, &m_ImageHeight, &m_NumberOfColorChannels, 0);

	if (!bytes) {
		throw std::runtime_error("Failed to load texture: " + fullPath);
	}

	glGenTextures(1, &m_TextureId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum format = GL_RGB;
	if (m_NumberOfColorChannels == 1)
		format = GL_RED;
	else if (m_NumberOfColorChannels == 3)
		format = GL_RGB;
	else if (m_NumberOfColorChannels == 4)
		format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, m_ImageWidth, m_ImageHeight, 0, format, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bytes);
}

Texture::~Texture() {
	glDeleteTextures(1, &m_TextureId);
}
