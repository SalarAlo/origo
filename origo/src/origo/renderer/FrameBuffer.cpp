#include "origo/renderer/FrameBuffer.h"
#include <stdexcept>

namespace Origo {

static inline GLint or_default(GLint v, GLint defv) { return v ? v : defv; }

FrameBuffer::FrameBuffer(const FrameBufferSpec& spec)
    : m_spec(spec) {
	invalidate();
}

FrameBuffer::~FrameBuffer() {
	destroy();
}

void FrameBuffer::destroy() {
	if (!m_color_tex.empty())
		glDeleteTextures((GLsizei)m_color_tex.size(), m_color_tex.data());
	if (m_depth_tex)
		glDeleteTextures(1, &m_depth_tex);
	if (m_fbo)
		glDeleteFramebuffers(1, &m_fbo);

	m_color_tex.clear();
	m_depth_tex = 0;
	m_fbo = 0;
}

void FrameBuffer::invalidate() {
	destroy();

	const bool multisampled = m_spec.Samples > 1;

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	m_color_tex.clear();
	m_depth_tex = 0;

	GLuint color_index = 0;

	for (const auto& a : m_spec.Attachments) {
		if (a.Type == AttachmentType::Color) {
			GLuint tex = 0;
			glGenTextures(1, &tex);

			if (multisampled) {
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
				glTexImage2DMultisample(
				    GL_TEXTURE_2D_MULTISAMPLE,
				    m_spec.Samples,
				    a.InternalFormat,
				    m_spec.Width,
				    m_spec.Height,
				    GL_TRUE);
				glFramebufferTexture2D(
				    GL_FRAMEBUFFER,
				    GL_COLOR_ATTACHMENT0 + color_index,
				    GL_TEXTURE_2D_MULTISAMPLE,
				    tex,
				    0);
			} else {
				glBindTexture(GL_TEXTURE_2D, tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, or_default(a.MinFilter, GL_LINEAR));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, or_default(a.MagFilter, GL_LINEAR));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, or_default(a.WrapS, GL_CLAMP_TO_EDGE));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, or_default(a.WrapT, GL_CLAMP_TO_EDGE));
				glTexImage2D(
				    GL_TEXTURE_2D,
				    0,
				    a.InternalFormat,
				    m_spec.Width,
				    m_spec.Height,
				    0,
				    a.Format,
				    a.TypeEnum,
				    nullptr);
				glFramebufferTexture2D(
				    GL_FRAMEBUFFER,
				    GL_COLOR_ATTACHMENT0 + color_index,
				    GL_TEXTURE_2D,
				    tex,
				    0);
			}

			m_color_tex.push_back(tex);
			color_index++;
		} else {
			if (m_depth_tex != 0)
				continue;

			glGenTextures(1, &m_depth_tex);

			GLenum attachment = (a.Type == AttachmentType::Depth)
			    ? GL_DEPTH_ATTACHMENT
			    : GL_DEPTH_STENCIL_ATTACHMENT;

			if (multisampled) {
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_depth_tex);
				glTexImage2DMultisample(
				    GL_TEXTURE_2D_MULTISAMPLE,
				    m_spec.Samples,
				    a.InternalFormat,
				    m_spec.Width,
				    m_spec.Height,
				    GL_TRUE);
				glFramebufferTexture2D(
				    GL_FRAMEBUFFER,
				    attachment,
				    GL_TEXTURE_2D_MULTISAMPLE,
				    m_depth_tex,
				    0);
			} else {
				glBindTexture(GL_TEXTURE_2D, m_depth_tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, or_default(a.MinFilter, GL_NEAREST));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, or_default(a.MagFilter, GL_NEAREST));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, or_default(a.WrapS, GL_CLAMP_TO_EDGE));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, or_default(a.WrapT, GL_CLAMP_TO_EDGE));
				glTexImage2D(
				    GL_TEXTURE_2D,
				    0,
				    a.InternalFormat,
				    m_spec.Width,
				    m_spec.Height,
				    0,
				    a.Format,
				    a.TypeEnum,
				    nullptr);
				glFramebufferTexture2D(
				    GL_FRAMEBUFFER,
				    attachment,
				    GL_TEXTURE_2D,
				    m_depth_tex,
				    0);
			}
		}
	}

	if (m_color_tex.empty()) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	} else {
		std::vector<GLenum> bufs(m_color_tex.size());
		for (size_t i = 0; i < bufs.size(); ++i)
			bufs[i] = GL_COLOR_ATTACHMENT0 + (GLenum)i;
		glDrawBuffers((GLsizei)bufs.size(), bufs.data());
		glReadBuffer(GL_COLOR_ATTACHMENT0);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		destroy();
		throw std::runtime_error("Framebuffer incomplete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void FrameBuffer::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(int width, int height) {
	if (width <= 0 || height <= 0)
		return;
	if (m_spec.Width == width && m_spec.Height == height)
		return;

	m_spec.Width = width;
	m_spec.Height = height;
	invalidate();
}

GLuint FrameBuffer::get_color_attachment(size_t index) const {
	return m_color_tex.at(index);
}

GLuint FrameBuffer::get_depth_attachment() const {
	return m_depth_tex;
}

void FrameBuffer::resolve_to(FrameBuffer& target) const {
	if (m_spec.Samples <= 1 || target.m_spec.Samples != 1)
		throw std::runtime_error("Invalid MSAA resolve");

	if (m_color_tex.empty() || target.m_color_tex.empty())
		throw std::runtime_error("Resolve requires color attachments");

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.m_fbo);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBlitFramebuffer(
	    0, 0, m_spec.Width, m_spec.Height,
	    0, 0, target.m_spec.Width, target.m_spec.Height,
	    GL_COLOR_BUFFER_BIT,
	    GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
