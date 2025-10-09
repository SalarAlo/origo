#pragma once

#include "origo/Shader.h"

#include "glm/ext/vector_float2.hpp"

namespace Origo {

struct SubTexture {
	glm::vec2 uvOffset; // (u0, v0) bottom-left corner
	glm::vec2 uvScale; // width/height in [0,1]
};

class TextureAtlas {
public:
	TextureAtlas(int width = 2048, int height = 2048);

	int AddImageFromFile(std::string_view filename);

	const SubTexture& GetSubTexture(int handle) const;

	void Bind(int slot = 0) const;
	void BindTexture(int handle, const Shader& shader) const;
	void Finalize() const;

private:
	GLuint m_TextureArrayId {};

	int m_Width;
	int m_Height;

	int m_CursorX {}; // current X position for row packing
	int m_CursorY {}; // current Y position
	int m_RowHeight {}; // tallest image in current row

	std::vector<SubTexture> m_SubTextures;
};

}
