#pragma once

#include "origo/core/Identifiable.h"
#include "origo/renderer/Shader.h"

namespace Origo {
enum class TextureType {
	Albedo,
};

int TextureTypeToSlot(TextureType);

class Texture : public Identifiable {
public:
	Texture(const std::string& path, TextureType type = TextureType::Albedo);
	void Bind(Ref<Shader>) const;
	TextureType GetType() const;

private:
	GLuint m_TextureId;
	TextureType m_Type;
};

}
