#include "origo/assets/Texture2D.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/Shader.h"
#include "origo/assets/TextureSource.h"

#include "origo/core/Logger.h"

#include "origo/renderer/GlDebug.h"

namespace Origo {
namespace {
const char* get_texture_uniform_name(TextureType textureType) {
	switch (textureType) {
	case TextureType::Albedo:
		return "u_texture_albedo";
	case TextureType::Normal:
		return "u_texture_normal";
	case TextureType::MetallicRoughness:
		return "u_texture_metallic_roughness";
	case TextureType::Ao:
		return "u_texture_ao";
	case TextureType::Emissive:
		return "u_texture_emissive";
	case TextureType::UI:
		return "u_texture_ui";
	}

	return "u_texture_albedo";
}
}

Texture2D::Texture2D(TextureType type)
    : m_texture_type(type)
    , m_texture_id(0) {
}

void Texture2D::set_wrap_mode(TextureWrapMode wrap_mode) {
	m_wrap_mode = wrap_mode;
	if (m_texture_id == 0)
		return;

	const GLint gl_wrap_mode = m_wrap_mode == TextureWrapMode::Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture_id));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap_mode));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap_mode));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::set_source(Scope<TextureSource> src) {
	m_source = std::move(src);
}

void Texture2D::resolve() {
	load();
}

void Texture2D::load() {
	if (!m_source) {
		ORG_ERROR("Texture::LoadCPU: No TextureSource assigned");
		return;
	}

	init_texture(m_source->get_initialisation_data());
	m_loaded = true;
}

void Texture2D::init_texture(const TextureInitialisationData& initData) {
	if (m_texture_id)
		return;

	GLCall(glGenTextures(1, &m_texture_id));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture_id));

	GLenum internal_format;
	GLenum data_format;

	if (initData.Channels == 4) {
		internal_format = GL_RGBA8;
		data_format = GL_RGBA;
	} else {
		internal_format = GL_RGB8;
		data_format = GL_RGB;
	}

	glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    internal_format,
	    initData.Width,
	    initData.Height,
	    0,
	    data_format,
	    GL_UNSIGNED_BYTE,
	    initData.PixelData.data());

	const GLint wrap_mode = m_wrap_mode == TextureWrapMode::Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, initData.GenerateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	if (initData.GenerateMipmaps)
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::bind(AssetHandle shaderId) const {
	if (!m_loaded) {
		const_cast<Texture2D*>(this)->load();
	}

	if (m_texture_id == 0) {
		ORG_ERROR("Texture::Bind: GPU texture not initialized");
		return;
	}

	auto shader = AssetManager::get_instance().get_asset<Shader>(shaderId);
	int slot = static_cast<int>(m_texture_type);

	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture_id));

	shader->set_uniform(get_texture_uniform_name(m_texture_type), slot);
}

}
