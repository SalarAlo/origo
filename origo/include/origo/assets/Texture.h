#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Asset.h"
#include "origo/assets/TextureSource.h"

namespace Origo {

enum class TextureType {
	Albedo,
};

class Texture : public Asset {
public:
	Texture(TextureType type = TextureType::Albedo);

	void SetSource(Scope<TextureSource> src);
	TextureSource* GetSource() const { return m_Source.get(); }

	void Bind(AssetHandle shaderId) const;

	AssetType GetAssetType() const override { return AssetType::Texture; }
	static AssetType GetClassAssetType() { return AssetType::Texture; }
	TextureType GetTextureType() const { return m_TextureType; }
	void SetTextureType(TextureType type) { m_TextureType = type; }

private:
	void InitTexture(int width, int height, int channels, unsigned char* data);
	void LoadCPU();
	void LoadGPU();

private:
	Scope<TextureSource> m_Source;

	GLuint m_TextureId = 0;
	TextureType m_TextureType;
};
}
