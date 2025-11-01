#include "origo/renderer/FrameBuffer.h"
#include "origo/core/Logger.h"
#include "origo/renderer/GlDebug.h"
#include <iostream>

namespace Origo {

FrameBuffer::FrameBuffer(int width, int height)
    : m_Width(width)
    , m_Height(height) {

	GLCall(glGenFramebuffers(1, &m_Id));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Id));

	GLCall(glGenTextures(1, &m_ColorTexId));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ColorTexId));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexId, 0));

	GLCall(glGenRenderbuffers(1, &m_DepthId));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthId));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthId));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		ORG_ERROR("[FrameBuffer] Incomplete framebuffer!");

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::Bind() {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Id));
	GLCall(glViewport(0, 0, m_Width, m_Height));
}

void FrameBuffer::Unbind() {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::Resize(int width, int height) {
	if (width == m_Width && height == m_Height)
		return;

	m_Width = width;
	m_Height = height;

	GLCall(glBindTexture(GL_TEXTURE_2D, m_ColorTexId));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthId));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
}
}
