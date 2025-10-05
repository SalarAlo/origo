#include "Mesh.h"
#include "engine/IndexBuffer.h"
#include "engine/Shader.h"
#include "engine/VertexArray.h"
#include "engine/VertexBuffer.h"

class Renderable {
public:
	Renderable(const Mesh& mesh);
	void Render(const Shader& shader);

private:
	Mesh m_Mesh {};

	VertexArray m_VertexArray {};
	VertexBuffer m_VertexBuffer { std::vector<float> {} };
	IndexBuffer m_IndexBuffer { std::vector<unsigned int> {} };
};
