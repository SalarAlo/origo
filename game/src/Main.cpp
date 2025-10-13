#include "origo/Camera.h"
#include "origo/events/EventTypes.h"
#include "origo/events/WindowEvent.h"
#include "origo/renderer/CubeVertices.h"
#include "origo/renderer/Renderer.h"
#include "origo/renderer/Shader.h"
#include "origo/renderer/Transform.h"
#include "origo/renderer/UniformList.hpp"
#include <memory>
#include <origo/Origo.h>
#include <magic_enum/magic_enum.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GameApplication : public Origo::Application {
public:
	GameApplication(const Origo::ApplicationSettings& applicationSettings)
	    : Origo::Application(applicationSettings)
	    , m_Shader("normal")
	    , m_Renderer()
	    , m_Camera(m_Window.GetAspectResolution(), { 0, 0, -3 }) {
	}

	void OnEvent(Origo::Event& event) override {
		static int count {};

		std::cout << "Received Event Nr. " << ++count << " of type " << magic_enum::enum_name(event.GetEventType()) << std::endl;

		if (event.GetEventType() == Origo::EventType::Window) {
			auto* winEvent { dynamic_cast<Origo::WindowEvent*>(&event) };
			if (winEvent->GetWindowEventType() == Origo::WindowEventType::WindowResize) {
				auto* winResizeEvent { dynamic_cast<Origo::WindowResizeEvent*>(winEvent) };
				auto size = winResizeEvent->GetSize();
				m_Camera.SetAspect(size.x / size.y);
			}
		}
	}

	void OnInit() override {
		Origo::Mesh cubeMesh { Origo::CUBE_VERTICES, Origo::CUBE_INDICES };

		Origo::UniformList list {};

		list.AddUniform("u_ProjectionMatrix", m_Camera.GetProjection());
		list.AddUniform("u_ViewMatrix", m_Camera.GetView());

		list.AddUniform("u_ViewPos", m_Camera.GetPosition());
		list.AddUniform("u_LightPos", m_Camera.GetPosition());

		list.AddUniform(
		    "u_ModelMatrix",
		    glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 1.0f)));

		Origo::Material cubeMaterial {
			MakeRef<Origo::UniformList>(std::move(list)),
			MakeRef<Origo::Shader>(m_Shader)
		};

		Origo::Transform transform {};

		m_Renderer.Submit(
		    MakeRef<Origo::Mesh>(cubeMesh),
		    MakeRef<Origo::Material>(cubeMaterial),
		    MakeRef<Origo::Transform>(transform));
	}

	void OnRender() override {
		m_Renderer.Flush();
	}

private:
	Origo::Shader m_Shader;
	Origo::Renderer m_Renderer {};
	Origo::Camera m_Camera {};
};

Origo::Application* Origo::CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./game",
		// i use i3. in my config apps with "ITSAFLOATER" title are floating.
		// thats why i call it like that
		.WindowSettings = { .Width = 1000, .Height = 1000, .Title = "ITSAFLOATER" }
	};

	return new GameApplication(settings);
}
