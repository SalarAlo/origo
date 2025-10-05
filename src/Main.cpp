#include "engine/Camera.h"
#include "engine/EventSystem.h"
#include "engine/IndexBuffer.h"
#include "engine/RenderTarget.h"
#include "engine/ScreenWindow.h"
#include "engine/CubeVertices.h"
#include "engine/Shader.h"
#include "engine/TextureAtlas.h"
#include "engine/VertexArray.h"
#include "engine/VertexBuffer.h"
#include "engine/VertexLayout.h"
#include "engine/Renderable.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

int main() {
	ScreenWindowSettings settings { .Width = 1900, .Height = 1060, .Title = "ITSAFLOATER" };
	ScreenWindow window { settings };

	Camera camera { window.GetAspectResolution(), { 0, 0, -3 } };

	EventSystem eventSystem { window };

	VertexLayout layout {};
	layout.AddAttribute<float>(3);
	layout.AddAttribute<float>(3);
	layout.AddAttribute<float>(2);

	Mesh CubeMesh {
		.Vertices = CUBE_VERTICES,
		.Indices = CUBE_INDICES,
		.Layout = layout
	};

	Renderable cube { CubeMesh };

	Shader shader { "normal" };

	TextureAtlas atlas {};
	int rowlettHandle { atlas.AddImageFromFile("rowlett.jpg") };
	atlas.Finalize();

	RenderTarget target(540, 540); // render at 640×360 internally

	int frames {};
	while (!window.ShouldClose()) {
		target.Bind();
		glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		atlas.Bind(0);

		shader.UseProgram();

		shader.SetUniformInt("u_Atlas", 0);
		atlas.BindTexture(rowlettHandle, shader);

		shader.SetUniformMat4("u_ProjectionMatrix", camera.GetProjection());
		shader.SetUniformMat4("u_ViewMatrix", camera.GetView());

		shader.SetUniformVec3("u_LightPos", glm::vec3 { 0.0f, 4.0f, -4.0f });
		shader.SetUniformVec3("u_ViewPos", camera.GetPosition());

		auto model { glm::mat4 { 1.0f } };
		model = glm::translate(model, { 0.0f, 1.0f, 0.0f });
		model = glm::rotate(model, frames * 0.01f, { 0.0f, 1.0f, 1.0f });
		shader.SetUniformMat4("u_ModelMatrix", model);

		cube.Render(shader);

		target.DrawToScreen(window.GetWidth(), window.GetHeight());

		window.SwapBuffers();
		eventSystem.CheckEvents();
		frames++;
	}

	return 0;
}
