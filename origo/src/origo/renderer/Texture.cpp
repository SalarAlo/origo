#include "origo/renderer/Texture.h"

namespace Origo {
Texture::Texture(std::string_view path, TextureType type)
    : m_Type(type) {
	glGenTextures(1, &m_TextureId);
}

};
