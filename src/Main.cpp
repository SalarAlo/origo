#include "engine/Camera.h"
#include "engine/EventSystem.h"
#include "engine/IndexBuffer.h"
#include "engine/ScreenWindow.h"
#include "engine/CubeVertices.h"
#include "engine/Shader.h"
#include "engine/Texture.h"
#include "engine/VertexArray.h"
#include "engine/VertexBuffer.h"
#include "engine/VertexLayout.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

int main() {
	ScreenWindowSettings settings { .Width = 1900, .Height = 1060, .Title = "ITSAFLOATER" };
	ScreenWindow window { settings };

	Camera camera { window.GetAspectResolution(), { 0, 0, -3 } };

	EventSystem eventSystem { window };

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
	Shader shader { "normal" };

	Texture text { "rowlett.jpg" };
	while (!window.ShouldClose()) {
		glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.UseProgram();

		shader.SetUniformMat4("u_ProjectionMatrix", camera.GetProjection());
		shader.SetUniformMat4("u_ViewMatrix", camera.GetView());

		shader.SetUniformVec3("u_LightPos", glm::vec3 { 0.0f, 4.0f, -4.0f });
		shader.SetUniformVec3("u_ViewPos", camera.GetPosition());

		auto model { glm::mat4 { 1.0f } };
		model = glm::translate(model, { 0.0f, 1.0f, 0.0f });
		shader.SetUniformMat4("u_ModelMatrix", model);

		glDrawElements(GL_TRIANGLES, indexBuffer.GetElementCount(), GL_UNSIGNED_INT, nullptr);

		window.SwapBuffers();
		eventSystem.CheckEvents();
	}

	return 0;
}
