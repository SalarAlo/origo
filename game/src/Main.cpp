
#include "origo/assets/PrimitiveShape.h"
#include "origo/assets/Shader.h"
#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"

#include "origo/input/Input.h"

#include "origo/scene/Transform.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Scene.h"

#include "origo/assets/AssetManager.h"

class GameApplication : public Origo::Application {
public:
	GameApplication(const Origo::ApplicationSettings& applicationSettings)
	    : Origo::Application(applicationSettings)
	    , m_Camera(m_Scene.GetCamera()) {
	}

	void Awake() override {
		m_Camera.SetSpeed(100);

		auto logoTexture { Origo::AssetManager::CreateAsset<Origo::Texture>("Rowlett", "rowlett.jpg") };

		m_Shader = Origo::AssetManager::CreateAsset<Origo::Shader>("Normal Shader", "normal");
		m_Material = Origo::AssetManager::CreateAsset<Origo::Material>("Normal Material", m_Shader, logoTexture);

		SpawnTestGrid();
	}

	void SpawnTestGrid() {
		auto cubeMesh = Origo::AssetManager::CreateAsset<Origo::Mesh>("Cube", Origo::PrimitiveShape::Cube);

		for (int i {}; i < GRID_SIZE; i++) {
			for (int j {}; j < GRID_SIZE; j++) {
				auto entity = m_Scene.CreateEntity(
				    "Cube_" + std::to_string(i * GRID_SIZE + j));

				Origo::Ref<Origo::Transform> transform {
					m_Scene.m_ComponentManager.AddComponent<Origo::Transform>(entity)
				};

				transform->SetPosition(glm::vec3 { i, 0, j });
				transform->SetScale(glm::vec3 { 1 });

				m_Scene.m_ComponentManager.AddComponent<Origo::MeshRenderer>(entity, m_Material, cubeMesh);
			}
		}
	}

	void HandleEvent(Origo::Event& event) override {
		m_Camera.OnEvent(event);
	}

	void Update(double dt) override {
		glm::vec3 direction(0.0f);
		// ORG_INFO("FPS: {}", 1.0 / dt);

		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_W))
			direction += m_Camera.GetForward();
		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_S))
			direction -= m_Camera.GetForward();
		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_D))
			direction += m_Camera.GetRight();
		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_A))
			direction -= m_Camera.GetRight();

		if (glm::length(direction) > 0.0f)
			m_Camera.Move(glm::normalize(direction) * static_cast<float>(dt));
	}

	void OnShutdown() override {
		Origo::SceneSerialization::Serialize("some.json", m_Scene);
	}

private:
	static constexpr int GRID_SIZE { 10 };

	Origo::Camera& m_Camera;

	Origo::Ref<Origo::Shader> m_Shader {};
	Origo::Ref<Origo::Material> m_Material {};
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
