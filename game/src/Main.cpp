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
		m_Camera->SetSpeed(10);

		auto logoTexture { Origo::AssetManager::CreateAsset<Origo::Texture>("Rowlett", "resources/textures/rowlett.jpg") };

		m_Shader = Origo::AssetManager::CreateAsset<Origo::Shader>("Normal Shader", "normal");
		m_Material = Origo::AssetManager::CreateAsset<Origo::Material>("Normal Material", m_Shader, logoTexture);

		SpawnTestGrid();
	}

	void SpawnTestGrid() {
		auto cubeMesh = Origo::AssetManager::CreateAsset<Origo::Mesh>("Cube", Origo::PrimitiveShape::Cube);

		for (int i {}; i < GRID_SIZE; i++) {
			for (int j {}; j < GRID_SIZE; j++) {
				auto entity = m_Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
				auto transform = m_Scene.AddComponent<Origo::Transform>(entity);
				transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });
				m_Scene.AddComponent<Origo::MeshRenderer>(entity, m_Material, cubeMesh);
			}
		}
	}

	void OnHandleEvent(Origo::Event& event) override {
		Origo::EventDispatcher dispatcher { event };

		dispatcher.Dispatch<Origo::WindowResizeEvent>([&](auto& resEvent) {
			auto size = resEvent.GetSize();
			m_Camera->SetAspectResolutino(static_cast<float>(size.x) / static_cast<float>(size.y));
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
				m_Camera->SetSensitivity(0);
			} else if (e.GetKeyPressed() == Origo::KeyboardKey::KEY_BACKSPACE) {
				Origo::Input::SetCursorMode(Origo::Input::CursorMode::Locked);
				m_Camera->SetSensitivity(0.1);
			}
		});
	}

	void OnUpdate(double dt) override {
		glm::vec3 direction(0.0f);
		constexpr float normalSpeed { 10 };
		constexpr float fastSpeed { normalSpeed * 5 };

		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_LEFT_SHIFT))
			m_Camera->SetSpeed(fastSpeed);
		else
			m_Camera->SetSpeed(normalSpeed);

		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_W))
			direction += m_Camera->GetForward();
		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_S))
			direction -= m_Camera->GetForward();
		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_D))
			direction += m_Camera->GetRight();
		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_A))
			direction -= m_Camera->GetRight();

		if (glm::length(direction) > 0.0f)
			m_Camera->Move(glm::normalize(direction) * static_cast<float>(dt));
	}

	void OnShutdown() override {
		Origo::JsonSerializer serializer { "./resources/scenes/some.json" };
		Origo::SceneSerialization::Serialize(m_Scene, serializer);
	}

	void OnImGuiRender() override {
	}

private:
	static constexpr int GRID_SIZE { 10 };

	Origo::Ref<Origo::Camera> m_Camera;
	Origo::Ref<Origo::Shader> m_Shader {};
	Origo::Ref<Origo::Material> m_Material {};
};

Origo::Application* Origo::CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./game",
		.WindowSettings = { .Width = 1800, .Height = 1000, .Title = "ITSAFLOATER" }
	};
	return new GameApplication(settings);
}
