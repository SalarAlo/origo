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

	void Bind() const;
	void Unbind() const;

	void AddData(const std::vector<unsigned int>& data);

	size_t GetElementCount() const { return m_Size; }

private:
	void UploadInitial();
	void UploadIncremental(size_t oldSize);

private:
	inline static GLuint s_CurrentlyBound = 0;

	GLuint m_BufferId {};
	unsigned int* m_Data = nullptr;
	size_t m_Size = 0;
	size_t m_Capacity = 0;

	size_t m_OldSize = 0;
};

}
