#pragma once

namespace Origo {
class IndexBuffer {
public:
	explicit IndexBuffer(const std::vector<unsigned int>& indices);

	void Bind() const;
	void Unbind() const;

	void AddData(const std::vector<unsigned int>& data);
	void ReplaceData(const std::vector<unsigned int>& data);

	std::size_t GetElementCount() const;

private:
	void SetDataOpenGL(bool initialUpload) const;

private:
	GLuint m_BufferId {};
	bool m_Bound;
	std::vector<unsigned int> m_Data;
};
}
