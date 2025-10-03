#include "engine/IndexBuffer.h"
#include "engine/ScreenWindow.h"
#include "engine/CubeVertices.h"
#include "engine/VertexArray.h"
#include "engine/VertexBuffer.h"
#include "engine/VertexLayout.hpp"

int main() {
	ScreenWindowSettings settings { .Width = 1900, .Height = 1060, .Title = "ITSAFLOATER" };
	ScreenWindow window { settings };

	VertexArray vertexArray {};
	vertexArray.Bind();

	VertexBuffer vertexBuffer { CUBE_VERTICES };
	vertexBuffer.Bind();

	IndexBuffer indexBuffer { CUBE_INDICES };
	indexBuffer.Bind();

	VertexLayout layout {};

	// position
	layout.AddAttribute<float>(3);
	// normals
	layout.AddAttribute<float>(3);
	// UV
	layout.AddAttribute<float>(2);

	vertexArray.ConnectBufferWithLayout(layout, vertexBuffer);

	while (!window.ShouldClose()) {
		window.SwapBuffers();
		glfwPollEvents();
	}

	return 0;
}
