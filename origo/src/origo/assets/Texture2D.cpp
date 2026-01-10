#include "origo/assets/Texture2D.h"

#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/TextureSource.h"
#include "origo/assets/Shader.h"

#include "origo/renderer/GlDebug.h"
#include "origo/core/Logger.h"

namespace Origo {

AssetHandle Texture2D::DefaultTexture() {
	static AssetHandle handle { [] {
		std::vector<unsigned char> whitePixel = { 255, 255, 255, 255 };

		auto handle { AssetFactory::CreateSyntheticAsset<Texture2D>("Default White Texture", UUID::FromArbitraryString("DEFAULT_TEXTURE_2D")) };
		auto tex = AssetManager::GetInstance().Get<Texture2D>(handle);
		tex->SetSource(MakeScope<TextureSourceRaw>(1, 1, 4, std::move(whitePixel), false));
		tex->Load();

		return handle;
	}() };

	return handle;
}

Texture2D::Texture2D(TextureType type)
    : m_TextureType(type)
    , m_TextureId(0) {
}

void Texture2D::SetSource(Scope<TextureSource> src) {
	m_Source = std::move(src);
}

void Texture2D::Resolve() {
	Load();
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
	if (m_TextureId)
		return;

	GLCall(glGenTextures(1, &m_TextureId));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	GLenum internalFormat;
	GLenum dataFormat;

	if (initData.Channels == 4) {
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	} else {
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    internalFormat,
	    initData.Width,
	    initData.Height,
	    0,
	    dataFormat,
	    GL_UNSIGNED_BYTE,
	    initData.PixelData.data());

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
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

	auto shader = AssetManager::GetInstance().Get<Shader>(shaderId);
	int slot = static_cast<int>(m_TextureType);

	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	std::string uniformName = "u_Texture_";
	uniformName += std::string(magic_enum::enum_name(m_TextureType));

	shader->SetUniform(uniformName, slot);
}

}
