#pragma once

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
	int Samples = 1;
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

	void bind() const;
	void unbind() const;

	void resize(int width, int height);

	int get_width() const { return m_spec.Width; }
	int get_height() const { return m_spec.Height; }
	int get_samples() const { return m_spec.Samples; }
	bool is_msaa() const { return m_spec.Samples > 1; }

	GLuint get_color_attachment(size_t index = 0) const;
	size_t get_color_attachment_count() const { return m_color_tex.size(); }
	const AttachmentSpec* get_color_attachment_spec(size_t index) const;
	GLuint get_depth_attachment() const;
	int read_pixel_int(size_t attachmentIndex, int x, int y) const;

	const FrameBufferSpec& get_specification() const { return m_spec; }

	void resolve_to(FrameBuffer& target) const;

private:
	void invalidate();
	void destroy();

private:
	FrameBufferSpec m_spec;

	GLuint m_fbo = 0;
	std::vector<GLuint> m_color_tex;
	GLuint m_depth_tex = 0;
};

}
