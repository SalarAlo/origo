#include <origo/Origo.h>

#include "origo/renderer/CubeVertices.h"

class GameApplication : public Origo::Application {
public:
	GameApplication(const Origo::ApplicationSettings& applicationSettings)
	    : Origo::Application(applicationSettings)
	    , m_Camera(m_Window.GetAspectResolution(), { 0, 0, -3 }) { }

	void OnEvent(Origo::Event& event) override {
		Origo::EventDispatcher dispatcher { event };

		dispatcher.Dipatch<Origo::WindowResizeEvent>([&](auto& resEvent) {
			auto size = resEvent.GetSize();
			m_Camera.SetAspect(size.x / size.y);
		});

		dispatcher.Dipatch<Origo::MouseMoveEvent>([&](Origo::MouseMoveEvent& resEvent) {
			auto size = resEvent.GetCoordinate();
			m_Transform->SetRotation(glm::vec3 { size.x / static_cast<float>(10), size.x / 4, size.y / static_cast<float>(10) });
		});
	}

	void OnInit() override {
		m_Shader = Origo::MakeRef<Origo::Shader>("normal");
		m_Material = Origo::MakeRef<Origo::Material>(m_Shader);

		auto m_Mesh = Origo::MakeRef<Origo::Mesh>(Origo::CUBE_VERTICES, Origo::CUBE_INDICES);

		auto cube { m_Scene.CreateEntity() };
		m_Transform = cube->AttachComponent<Origo::Transform>();
		m_Transform->SetPosition(glm::vec3 { 0, 0, 3 });

		cube->AttachComponent<Origo::MeshRenderer>(m_Material, m_Mesh);
	}

private:
	Origo::Camera m_Camera {};

	Origo::Ref<Origo::Shader> m_Shader {};
	Origo::Ref<Origo::Material> m_Material {};
	Origo::Ref<Origo::Transform> m_Transform {};
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
