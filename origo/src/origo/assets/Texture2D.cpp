#include "origo/assets/Texture2D.h"

#include "origo/assets/TextureSource.h"
#include "origo/assets/Shader.h"

#include "origo/renderer/GlDebug.h"
#include "origo/core/Logger.h"

namespace Origo {

Texture2D::Texture2D(TextureType type)
    : m_TextureType(type)
    , m_TextureId(0) {
}

void Texture2D::SetSource(Scope<TextureSource> src) {
	m_Source = std::move(src);
}

void Texture2D::Load() {
	if (!m_Source) {
		ORG_ERROR("Texture::LoadCPU: No TextureSource assigned");
		return;
	}

	InitTexture(m_Source->GetInitialisationData());
	m_Loaded = true;
}

void Texture2D::InitTexture(const TextureInitialisationData& initData) {
	GLCall(glGenTextures(1, &m_TextureId));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	GLenum format = (initData.Channels == 4 ? GL_RGBA : GL_RGB);

	GLCall(glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    format,
	    initData.Width,
	    initData.Height,
	    0,
	    format,
	    GL_UNSIGNED_BYTE,
	    initData.PixelData.data()));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, initData.GenerateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	if (initData.GenerateMipmaps)
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::Bind(AssetHandle shaderId) const {
	if (!m_Loaded) {
		const_cast<Texture2D*>(this)->Load();
	}

	if (m_TextureId == 0) {
		ORG_ERROR("Texture::Bind: GPU texture not initialized");
		return;
	}

	auto shader = AssetManagerFast::GetInstance().Get<Shader>(shaderId);
	int slot = static_cast<int>(m_TextureType);

	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	std::string uniformName = "u_Texture_";
	uniformName += std::string(magic_enum::enum_name(m_TextureType));

	shader->SetUniform(uniformName, slot);
}

}
