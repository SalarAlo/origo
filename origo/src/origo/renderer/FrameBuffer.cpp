#include "origo/renderer/FrameBuffer.h"
#include <stdexcept>

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
	if (!m_ColorTex.empty())
		glDeleteTextures((GLsizei)m_ColorTex.size(), m_ColorTex.data());
	if (m_DepthTex)
		glDeleteTextures(1, &m_DepthTex);
	if (m_FBO)
		glDeleteFramebuffers(1, &m_FBO);

	m_ColorTex.clear();
	m_DepthTex = 0;
	m_FBO = 0;
}

void FrameBuffer::Invalidate() {
	Destroy();

	const bool multisampled = m_Spec.Samples > 1;

	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	m_ColorTex.clear();
	m_DepthTex = 0;

	GLuint colorIndex = 0;

	for (const auto& a : m_Spec.Attachments) {
		if (a.Type == AttachmentType::Color) {
			GLuint tex = 0;
			glGenTextures(1, &tex);

			if (multisampled) {
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
				glTexImage2DMultisample(
				    GL_TEXTURE_2D_MULTISAMPLE,
				    m_Spec.Samples,
				    a.InternalFormat,
				    m_Spec.Width,
				    m_Spec.Height,
				    GL_TRUE);
				glFramebufferTexture2D(
				    GL_FRAMEBUFFER,
				    GL_COLOR_ATTACHMENT0 + colorIndex,
				    GL_TEXTURE_2D_MULTISAMPLE,
				    tex,
				    0);
			} else {
				glBindTexture(GL_TEXTURE_2D, tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OrDefault(a.MinFilter, GL_LINEAR));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OrDefault(a.MagFilter, GL_LINEAR));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OrDefault(a.WrapS, GL_CLAMP_TO_EDGE));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OrDefault(a.WrapT, GL_CLAMP_TO_EDGE));
				glTexImage2D(
				    GL_TEXTURE_2D,
				    0,
				    a.InternalFormat,
				    m_Spec.Width,
				    m_Spec.Height,
				    0,
				    a.Format,
				    a.TypeEnum,
				    nullptr);
				glFramebufferTexture2D(
				    GL_FRAMEBUFFER,
				    GL_COLOR_ATTACHMENT0 + colorIndex,
				    GL_TEXTURE_2D,
				    tex,
				    0);
			}

			m_ColorTex.push_back(tex);
			colorIndex++;
		} else {
			if (m_DepthTex != 0)
				continue;

			glGenTextures(1, &m_DepthTex);

			GLenum attachment = (a.Type == AttachmentType::Depth)
			    ? GL_DEPTH_ATTACHMENT
			    : GL_DEPTH_STENCIL_ATTACHMENT;

			if (multisampled) {
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthTex);
				glTexImage2DMultisample(
				    GL_TEXTURE_2D_MULTISAMPLE,
				    m_Spec.Samples,
				    a.InternalFormat,
				    m_Spec.Width,
				    m_Spec.Height,
				    GL_TRUE);
				glFramebufferTexture2D(
				    GL_FRAMEBUFFER,
				    attachment,
				    GL_TEXTURE_2D_MULTISAMPLE,
				    m_DepthTex,
				    0);
			} else {
				glBindTexture(GL_TEXTURE_2D, m_DepthTex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OrDefault(a.MinFilter, GL_NEAREST));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OrDefault(a.MagFilter, GL_NEAREST));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OrDefault(a.WrapS, GL_CLAMP_TO_EDGE));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OrDefault(a.WrapT, GL_CLAMP_TO_EDGE));
				glTexImage2D(
				    GL_TEXTURE_2D,
				    0,
				    a.InternalFormat,
				    m_Spec.Width,
				    m_Spec.Height,
				    0,
				    a.Format,
				    a.TypeEnum,
				    nullptr);
				glFramebufferTexture2D(
				    GL_FRAMEBUFFER,
				    attachment,
				    GL_TEXTURE_2D,
				    m_DepthTex,
				    0);
			}
		}
	}

	if (m_ColorTex.empty()) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	} else {
		std::vector<GLenum> bufs(m_ColorTex.size());
		for (size_t i = 0; i < bufs.size(); ++i)
			bufs[i] = GL_COLOR_ATTACHMENT0 + (GLenum)i;
		glDrawBuffers((GLsizei)bufs.size(), bufs.data());
		glReadBuffer(GL_COLOR_ATTACHMENT0);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Destroy();
		throw std::runtime_error("Framebuffer incomplete");
	}

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

void FrameBuffer::ResolveTo(FrameBuffer& target) const {
	if (m_Spec.Samples <= 1 || target.m_Spec.Samples != 1)
		throw std::runtime_error("Invalid MSAA resolve");

	if (m_ColorTex.empty() || target.m_ColorTex.empty())
		throw std::runtime_error("Resolve requires color attachments");

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.m_FBO);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBlitFramebuffer(
	    0, 0, m_Spec.Width, m_Spec.Height,
	    0, 0, target.m_Spec.Width, target.m_Spec.Height,
	    GL_COLOR_BUFFER_BIT,
	    GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
