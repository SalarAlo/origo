#pragma once

namespace Origo {
class IndexBuffer {
public:
	explicit IndexBuffer(const std::vector<unsigned int>& indices);

	void Bind() const;
	void Unbind() const;

	std::size_t GetElementCount() const;

private:
	GLuint m_BufferId {};
	std::size_t m_ElementCount {};
};
}
