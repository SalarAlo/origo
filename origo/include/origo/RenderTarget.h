#pragma once

class RenderTarget {
public:
	RenderTarget(int width, int height);
	~RenderTarget();

	void Bind();
	void Unbind();
	void DrawToScreen(int screenWidth, int screenHeight);

private:
	GLuint m_Fbo {};
	GLuint m_ColorTex {};
	GLuint m_DepthRbo {};
	int m_Width {};
	int m_Height {};
};
