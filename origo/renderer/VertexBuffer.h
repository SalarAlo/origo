#pragma once

namespace Origo {
class VertexBuffer {
public:
	explicit VertexBuffer(const std::vector<float>& data);
	~VertexBuffer();

	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	VertexBuffer(VertexBuffer&&) = delete;
	VertexBuffer& operator=(VertexBuffer&&) = delete;

	void bind() const;
	void unbind() const;

	void add_data(const void* data, size_t size);

private:
	void set_data_open_gl(bool initialUpload);
	void resize(size_t newSize);

private:
	inline static GLuint s_currently_bound { 0 };

	void* m_data {};
	size_t m_size {};
	size_t m_capacity {};
	size_t m_size_before_last_append = 0;

	GLuint m_buffer_id {};
};
}
