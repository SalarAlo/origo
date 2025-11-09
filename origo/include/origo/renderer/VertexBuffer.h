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

	void BindTemp() const;
	void UnbindTemp() const;

	void AddData(const std::vector<float>& data);
	void ReplaceData(const std::vector<float>& data);

private:
	void SetDataOpenGL(bool initialUpload);

private:
	inline static GLuint s_CurrentlyBound { 0 };
	std::vector<float> m_Data {};
	GLuint m_BufferId {};
	size_t m_Capacity {};
};
}
