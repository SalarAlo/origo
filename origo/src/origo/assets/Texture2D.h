#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Asset.h"
#include "origo/assets/TextureSource.h"

namespace Origo {

enum class TextureType {
	Albedo,
	UI
};

class Texture2D : public Asset {
public:
	Texture2D(TextureType type = TextureType::Albedo);

	void SetSource(Scope<TextureSource> src);
	TextureSource* GetSource() const { return m_Source.get(); }

	void Bind(AssetHandle shaderId) const;

	AssetType GetAssetType() const override { return AssetType::Texture2D; }
	static AssetType GetClassAssetType() { return AssetType::Texture2D; }

	TextureType GetTextureType() const { return m_TextureType; }
	void SetTextureType(TextureType type) { m_TextureType = type; }
	GLuint GetRendererID() { return m_TextureId; }

	void Load();

private:
	void InitTexture(const TextureInitialisationData& data);

private:
	Scope<TextureSource> m_Source;

	GLuint m_TextureId = 0;
	bool m_Loaded { false };
	TextureType m_TextureType;
};
}
