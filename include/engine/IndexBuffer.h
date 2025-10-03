#pragma once

class IndexBuffer {
public:
	explicit IndexBuffer(const std::vector<unsigned int>& indices);

	void Bind() const;
	void Unbind() const;

private:
	GLuint m_BufferId {};
};
