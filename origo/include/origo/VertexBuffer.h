#pragma once

class VertexBuffer {
public:
	explicit VertexBuffer(std::vector<float> data);

	void Bind() const;
	void Unbind() const;

private:
	GLuint m_BufferId {};
};
