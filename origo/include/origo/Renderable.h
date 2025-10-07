#include "Mesh.h"
#include "origo/IndexBuffer.h"
#include "origo/Shader.h"
#include "origo/VertexArray.h"
#include "origo/VertexBuffer.h"

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
