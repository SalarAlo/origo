#pragma once

namespace Origo {
enum class VertexAttributeSemantic {
	None,
	Position,
	Normal,
	TexCoord,
	Tangent
};

struct VertexAttribute {
	VertexAttributeSemantic Semantic;
	GLuint Type { GL_FLOAT };
	GLuint Amount { 0 };
	GLint Normalized { GL_FALSE };
	void* Offset { nullptr };
};
}
