#define STB_IMAGE_IMPLEMENTATION

#include "origo/renderer/TextureAtlas.h"
#include <stb_image.h>

namespace Origo {
TextureAtlas::TextureAtlas(int width, int height)
    : m_Width(width)
    , m_Height(height) {
	glGenTextures(1, &m_TextureArrayId);

	glBindTexture(GL_TEXTURE_2D, m_TextureArrayId);

	// allocate empty atlas
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
	    width, height,
	    0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	stbi_set_flip_vertically_on_load(true);
}

int TextureAtlas::AddImageFromFile(std::string_view filename) {
	auto handle { m_SubTextures.size() };

	int imageWidth {};
	int imageHeight {};

	auto fullPath { std::string("./resources/textures/") + std::string(filename) };
	unsigned char* bytes = stbi_load(fullPath.data(), &imageWidth, &imageHeight, nullptr, STBI_rgb_alpha);

	if (!bytes) {
		throw std::runtime_error("Failed to load texture: " + fullPath);
	}

	if (m_CursorX + imageWidth > m_Width) {
		m_CursorX = 0;
		m_CursorY += m_RowHeight;
		m_RowHeight = 0;
	}
	if (m_CursorY + imageHeight > m_Height) {
		stbi_image_free(bytes);
		throw std::runtime_error("TextureAtlas full: " + fullPath);
	}

	// --- Upload into atlas ---
	glBindTexture(GL_TEXTURE_2D, m_TextureArrayId);
	glTexSubImage2D(GL_TEXTURE_2D, 0,
	    m_CursorX, m_CursorY,
	    imageWidth, imageHeight,
	    GL_RGBA, GL_UNSIGNED_BYTE, bytes);

	// --- UV mapping (normalized) ---
	SubTexture sub;
	sub.uvOffset = glm::vec2(
	    static_cast<float>(m_CursorX) / m_Width,
	    static_cast<float>(m_CursorY) / m_Height);
	sub.uvScale = glm::vec2(
	    static_cast<float>(imageWidth) / m_Width,
	    static_cast<float>(imageHeight) / m_Height);

	m_SubTextures.push_back(sub);

	// --- Advance cursor ---
	m_CursorX += imageWidth;
	m_RowHeight = std::max(m_RowHeight, imageHeight);

	stbi_image_free(bytes);

	return handle;
}

void TextureAtlas::Bind(int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_TextureArrayId);
}

const SubTexture& TextureAtlas::GetSubTexture(int handle) const {
	return m_SubTextures[handle];
}

void TextureAtlas::BindTexture(int handle, const Shader& shader) const {
	shader.SetUniform("u_UvOffset", m_SubTextures[handle].uvOffset);
	shader.SetUniform("u_UvScale", m_SubTextures[handle].uvScale);
}

void TextureAtlas::Finalize() const {
	glBindTexture(GL_TEXTURE_2D, m_TextureArrayId);
	glGenerateMipmap(GL_TEXTURE_2D);
}
}
