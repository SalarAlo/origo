#pragma once

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

	std::string GetPath() const {
		return m_Path;
	}

private:
	GLuint m_TextureId;
	TextureType m_TextureType;
	std::string m_Path;
};

}
