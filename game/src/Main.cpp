#include "origo/assets/AssetManager.h"
#include "origo/assets/PrimitiveShape.h"
#include "origo/assets/Shader.h"

#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"

#include "origo/events/Event.h"
#include "origo/events/KeyEvent.h"
#include "origo/events/MouseEvent.h"
#include "origo/events/WindowEvent.h"
#include "origo/input/Input.h"

#include "origo/renderer/Renderer.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"
#include "origo/scene/Scene.h"

#include "origo/serialization/JsonSerializer.h"

class GameApplication : public Origo::Application {
public:
	GameApplication(const Origo::ApplicationSettings& applicationSettings)
	    : Origo::Application(applicationSettings)
	    , m_Camera(m_Scene.GetMainCamera()) {
	}

	void OnAwake() override {
		auto logoTexture { Origo::AssetManager::CreateAsset<Origo::Texture>("Rowlett", "resources/textures/origo_logo.png") };

		m_Shader = Origo::AssetManager::CreateAsset<Origo::Shader>("Normal Shader", "normal");
		m_Material = Origo::AssetManager::CreateAsset<Origo::Material>("Normal Material", m_Shader, logoTexture);

		SpawnTestGrid();
	}

	void SpawnTestGrid() {
		auto cubeMesh = Origo::AssetManager::CreateAsset<Origo::Mesh>("Cube", Origo::PrimitiveShape::Cube);

		for (int i {}; i < GRID_SIZE; i++) {
			for (int j {}; j < GRID_SIZE; j++) {
				auto entity = m_Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
				Origo::Transform* transform = m_Scene.AddComponent<Origo::Transform>(entity);
				transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });
				m_Scene.AddComponent<Origo::MeshRenderer>(entity, m_Material, cubeMesh);
			}
		}
	}

	void OnEvent(Origo::Event& event) override {
		Origo::EventDispatcher dispatcher { event };

		dispatcher.Dispatch<Origo::WindowResizeEvent>([&](auto& resEvent) {
			auto size = resEvent.GetSize();
			m_Camera->SetAspectResolution(static_cast<float>(size.x) / static_cast<float>(size.y));
		});

		dispatcher.Dispatch<Origo::MouseMoveEvent>([&](Origo::MouseMoveEvent& moveEvent) {
			static float m_Sensitivity {};
			const auto& coords = moveEvent.GetCoordinate();
			static glm::vec2 m_LastMousePos {};

			m_LastMousePos = coords;

			float dx = m_LastMousePos.x - coords.x;
			float dy = m_LastMousePos.y - coords.y;

			m_Camera->Rotate(-dx * m_Sensitivity, dy * m_Sensitivity);

			m_LastMousePos = coords;
		});

		dispatcher.Dispatch<Origo::KeyPressEvent>([&](Origo::KeyPressEvent& e) {
			if (e.GetKeyPressed() == Origo::KeyboardKey::KEY_ESCAPE) {
				Origo::Input::SetCursorMode(Origo::Input::CursorMode::Free);
			} else if (e.GetKeyPressed() == Origo::KeyboardKey::KEY_BACKSPACE) {
				Origo::Input::SetCursorMode(Origo::Input::CursorMode::Locked);
			}
		});
	}

	void OnShutdown() override {
		Origo::JsonSerializer serializer { "./resources/scenes/some.json" };
		Origo::SceneSerialization::Serialize(m_Scene, serializer);
	}

private:
	static constexpr int GRID_SIZE { 10 };

	Origo::Camera* m_Camera;
	Origo::Shader* m_Shader {};
	Origo::Material* m_Material {};
};

Origo::Application* Origo::CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./game",
		.WindowSettings = { .Width = 1800, .Height = 1000, .Title = "ITSAFLOATER" }
	};
	return new GameApplication(settings);
}
