#pragma once

namespace Origo {
class FrameBuffer {
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	FrameBuffer(FrameBuffer&&) = delete;
	FrameBuffer& operator=(FrameBuffer&&) = delete;

	void Bind();
	void Unbind();
	void Resize(int width, int height);

	GLuint GetColorTexHandle() { return m_ColorTexId; }

	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

private:
	int m_Width;
	int m_Height;

	GLuint m_Id;
	GLuint m_ColorTexId;
	GLuint m_DepthId;
};
}
