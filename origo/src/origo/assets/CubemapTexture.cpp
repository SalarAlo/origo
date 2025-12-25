#include "origo/assets/CubemapTexture.h"
#include "origo/assets/TextureSource.h"

namespace Origo {
CubemapTexture::CubemapTexture(CubemapDescription description)
    : m_Description(description) {
}

void CubemapTexture::Load() {
	GLCall(glGenTextures(1, &m_TextureID));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID));

	const auto& textures = m_Description.GetPaths();

	for (size_t i = 0; i < 6; i++) {
		TextureSourceFile source(textures[i].c_str(), false);
		auto initData = source.GetInitialisationData();

		GLenum format = (initData.Channels == 4 ? GL_RGBA : GL_RGB);

		GLCall(glTexImage2D(
		    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		    0,
		    format,
		    initData.Width,
		    initData.Height,
		    0,
		    format,
		    GL_UNSIGNED_BYTE,
		    initData.PixelData.data()));
	}

	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
}

void CubemapTexture::Bind(uint32_t slot) const {
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID));
}

}
