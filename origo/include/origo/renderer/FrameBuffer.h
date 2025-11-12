#pragma once
#include <vector>
#include <glad/glad.h>

namespace Origo {

enum class AttachmentType {
	Color,
	Depth,
	DepthStencil
};

struct AttachmentSpec {
	AttachmentType Type;

	GLint InternalFormat;
	GLenum Format;
	GLenum TypeEnum;

	GLint MinFilter = GL_LINEAR;
	GLint MagFilter = GL_LINEAR;
	GLint WrapS = GL_CLAMP_TO_EDGE;
	GLint WrapT = GL_CLAMP_TO_EDGE;
};

struct FrameBufferSpec {
	int Width = 1;
	int Height = 1;
	std::vector<AttachmentSpec> Attachments;
};

class FrameBuffer {
public:
	explicit FrameBuffer(const FrameBufferSpec& spec);
	~FrameBuffer();

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = delete;
	FrameBuffer& operator=(FrameBuffer&&) = delete;

	void Bind() const;
	void Unbind() const;

	void Resize(int width, int height);
	int GetWidth() const { return m_Spec.Width; }
	int GetHeight() const { return m_Spec.Height; }

	GLuint GetColorAttachment(size_t index = 0) const;
	GLuint GetDepthAttachment() const;
	const FrameBufferSpec& GetSpecification() const { return m_Spec; }

private:
	void Invalidate();
	void Destroy();

private:
	FrameBufferSpec m_Spec;

	GLuint m_FBO = 0;
	std::vector<GLuint> m_ColorTex;
	GLuint m_DepthTex = 0;
};

}
