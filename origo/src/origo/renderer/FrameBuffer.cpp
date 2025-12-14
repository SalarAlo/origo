#include "origo/renderer/FrameBuffer.h"

namespace Origo {

static inline GLint OrDefault(GLint v, GLint defv) { return v ? v : defv; }

FrameBuffer::FrameBuffer(const FrameBufferSpec& spec)
    : m_Spec(spec) {
	Invalidate();
}

FrameBuffer::~FrameBuffer() {
	Destroy();
}

void FrameBuffer::Destroy() {
	glDeleteTextures((GLsizei)m_ColorTex.size(), m_ColorTex.data());
	glDeleteTextures(1, &m_DepthTex);
	glDeleteFramebuffers(1, &m_FBO);

	m_DepthTex = m_FBO = 0;
	m_ColorTex.clear();
}

void FrameBuffer::Invalidate() {
	Destroy();

	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	m_ColorTex.clear();
	m_DepthTex = 0;

	GLuint colorIndex = 0;
	for (const auto& a : m_Spec.Attachments) {
		if (a.Type == AttachmentType::Color) {
			GLuint tex = 0;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OrDefault(a.MinFilter, GL_LINEAR));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OrDefault(a.MagFilter, GL_LINEAR));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OrDefault(a.WrapS, GL_CLAMP_TO_EDGE));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OrDefault(a.WrapT, GL_CLAMP_TO_EDGE));
			glTexImage2D(GL_TEXTURE_2D, 0, a.InternalFormat,
			    m_Spec.Width, m_Spec.Height, 0,
			    a.Format, a.TypeEnum, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER,
			    (GLenum)(GL_COLOR_ATTACHMENT0 + colorIndex),
			    GL_TEXTURE_2D, tex, 0);
			m_ColorTex.push_back(tex);
			colorIndex++;
		} else {
			if (m_DepthTex == 0) {
				glGenTextures(1, &m_DepthTex);
				glBindTexture(GL_TEXTURE_2D, m_DepthTex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OrDefault(a.MinFilter, GL_NEAREST));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OrDefault(a.MagFilter, GL_NEAREST));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OrDefault(a.WrapS, GL_CLAMP_TO_EDGE));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OrDefault(a.WrapT, GL_CLAMP_TO_EDGE));
				glTexImage2D(GL_TEXTURE_2D, 0, a.InternalFormat,
				    m_Spec.Width, m_Spec.Height, 0,
				    a.Format, a.TypeEnum, nullptr);
				GLenum attach = (a.Type == AttachmentType::Depth)
				    ? GL_DEPTH_ATTACHMENT
				    : GL_DEPTH_STENCIL_ATTACHMENT;
				glFramebufferTexture2D(GL_FRAMEBUFFER, attach,
				    GL_TEXTURE_2D, m_DepthTex, 0);
			}
		}
	}

	if (m_ColorTex.empty()) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	} else {
		std::vector<GLenum> bufs(m_ColorTex.size());
		for (size_t i = 0; i < bufs.size(); ++i)
			bufs[i] = (GLenum)(GL_COLOR_ATTACHMENT0 + i);
		glDrawBuffers((GLsizei)bufs.size(), bufs.data());
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Destroy();
		throw std::runtime_error("Framebuffer incomplete (GL 3.3).");
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void FrameBuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Resize(int width, int height) {
	if (width <= 0 || height <= 0)
		return;
	if (m_Spec.Width == width && m_Spec.Height == height)
		return;
	m_Spec.Width = width;
	m_Spec.Height = height;
	Invalidate();
}

GLuint FrameBuffer::GetColorAttachment(size_t index) const {
	return m_ColorTex.at(index);
}

GLuint FrameBuffer::GetDepthAttachment() const {
	return m_DepthTex;
}

}
