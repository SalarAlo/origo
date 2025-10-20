#pragma once

#include "origo/assets/Mesh.h"
#include "origo/assets/Shader.h"

namespace Origo {
enum class TextureType {
	Albedo,
};

int TextureTypeToSlot(TextureType);

class Texture : public Asset {
public:
	Texture(const std::string& path, TextureType type = TextureType::Albedo);
	void Bind(Ref<Shader>) const;
	TextureType GetType() const;

	AssetType GetAssetType() const override {
		return AssetType::Texture;
	}

private:
	GLuint m_TextureId;
	TextureType m_TextureType;
};

}
