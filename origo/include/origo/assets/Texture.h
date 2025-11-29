#pragma once

#include "assimp/texture.h"
#include "origo/assets/Asset.h"
#include "origo/assets/TextureSource.h"
#include "origo/core/RID.h"

namespace Origo {

// Texture type enum index corrosponds to shader slot bound to
enum class TextureType {
	Albedo,
};

class Texture : public Asset {
public:
	Texture(const std::string& path, TextureType type = TextureType::Albedo);
	Texture(const aiTexture* embeddedTex, TextureType type = TextureType::Albedo);

	void Bind(RID shaderId) const;

	TextureType GetTextureType() const { return m_TextureType; }

	AssetType GetAssetType() const override { return AssetType::Texture; }
	static AssetType GetClassAssetType() { return AssetType::Texture; }

	TextureSource* GetSource() const { return m_Source.get(); }

private:
	void InitTexture(int width, int height, int channels, unsigned char* data);

private:
	Scope<TextureSource> m_Source;

	GLuint m_TextureId;
	TextureType m_TextureType;
};

}
