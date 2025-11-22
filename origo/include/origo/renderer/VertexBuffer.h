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

	void Bind() const;
	void Unbind() const;

	void AddData(const void* data, size_t size);

private:
	void SetDataOpenGL(bool initialUpload);
	void Resize(size_t newSize);

private:
	inline static GLuint s_CurrentlyBound { 0 };

	void* m_Data {};
	size_t m_Size {};
	size_t m_Capacity {};
	size_t m_SizeBeforeLastAppend = 0;

	GLuint m_BufferId {};
};
}
