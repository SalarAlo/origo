#pragma once

namespace Origo {

class IndexBuffer {
public:
	explicit IndexBuffer(const std::vector<unsigned int>& indices);
	~IndexBuffer();

	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	IndexBuffer(IndexBuffer&&) = delete;
	IndexBuffer& operator=(IndexBuffer&&) = delete;

	void bind() const;
	void unbind() const;

	void add_data(const std::vector<unsigned int>& data);

	size_t get_element_count() const { return m_size; }

private:
	void upload_initial();
	void upload_incremental(size_t oldSize);

private:
	inline static GLuint s_currently_bound = 0;

	GLuint m_buffer_id {};
	unsigned int* m_data = nullptr;
	size_t m_size = 0;
	size_t m_capacity = 0;

	size_t m_old_size = 0;
};

}
