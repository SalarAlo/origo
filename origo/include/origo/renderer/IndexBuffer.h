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

	void BindTemp() const;
	void UnbindTemp() const;

	void AddData(const std::vector<unsigned int>& data);
	void ReplaceData(const std::vector<unsigned int>& data);

	std::size_t GetElementCount() const;

private:
	void SetDataOpenGL();

private:
	inline static GLuint s_CurrentlyBound { false };
	std::vector<unsigned int> m_Data;
	GLuint m_BufferId {};
	size_t m_Capacity {};
};
}
