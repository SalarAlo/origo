#pragma once

namespace Origo {
class VertexBuffer {
public:
	explicit VertexBuffer(const std::vector<float>& data);

	void Bind() const;
	void Unbind() const;
	void AddData(const std::vector<float>& data);
	void ReplaceData(const std::vector<float>& data);

private:
	void SetDataOpenGL(bool initialUpload) const;

private:
	std::vector<float> m_Data;
	GLuint m_BufferId {};
};
}
