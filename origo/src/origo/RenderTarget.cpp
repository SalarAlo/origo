#include "origo/RenderTarget.h"

RenderTarget::RenderTarget(int width, int height)
    : m_Width(width)
    , m_Height(height)
    , m_Fbo(0)
    , m_ColorTex(0)
    , m_DepthRbo(0) {
	glGenFramebuffers(1, &m_Fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	glGenTextures(1, &m_ColorTex);
	glBindTexture(GL_TEXTURE_2D, m_ColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
	    GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTex, 0);

	glGenRenderbuffers(1, &m_DepthRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthRbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "RenderTarget: Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget() {
	if (m_DepthRbo)
		glDeleteRenderbuffers(1, &m_DepthRbo);
	if (m_ColorTex)
		glDeleteTextures(1, &m_ColorTex);
	if (m_Fbo)
		glDeleteFramebuffers(1, &m_Fbo);
}

void RenderTarget::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
	glViewport(0, 0, m_Width, m_Height);
	glEnable(GL_DEPTH_TEST);
}

void RenderTarget::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::DrawToScreen(int screenWidth, int screenHeight) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// set this as copy
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Fbo);
	// default window back frame buffer = id 0
	// set this as source
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	Bind();
}
