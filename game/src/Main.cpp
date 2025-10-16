#include "origo/core/EntryPoint.h"

#include "origo/assets/PrimitiveShapes.h"
#include "origo/core/Application.h"
#include "origo/events/KeyEvent.h"
#include "origo/input/Input.h"
#include "origo/renderer/ModelLoader.h"
#include "origo/renderer/Texture.h"
#include "origo/scene/MeshRenderer.h"

class GameApplication : public Origo::Application {
public:
	GameApplication(const Origo::ApplicationSettings& applicationSettings)
	    : Origo::Application(applicationSettings)
	    , m_Camera(m_Scene.GetCamera()) {
	}

	void Awake() override {
		m_Camera.SetSpeed(100);

		Origo::Ref<Origo::Texture> logo { Origo::MakeRef<Origo::Texture>("resources/textures/rowlett.jpg") };

		m_Shader = Origo::MakeRef<Origo::Shader>("normal");
		m_Material = Origo::MakeRef<Origo::Material>(m_Shader, logo);

		SpawnTestGrid();
	}

	void SpawnTestGrid() {
		auto meshes = Origo::ModelLoader::LoadModel("pikachu.glb");
		auto cubeMesh = Origo::Assets::LoadShape(Origo::Assets::PrimitiveShape::CUBE);
		for (int i {}; i < GRID_SIZE; i++) {
			for (int j {}; j < GRID_SIZE; j++) {
				for (const auto& mesh : meshes) {
					auto entity = m_Scene.CreateEntity();
					auto transform { entity->AttachComponent<Origo::Transform>() };
					transform->SetPosition(glm::vec3 { i * 10, j * 30, 11 });
					transform->SetScale(glm::vec3 { 10 });

					entity->AttachComponent<Origo::MeshRenderer>(m_Material, cubeMesh);
				}
			}
		}
	}

	void HandleEvent(Origo::Event& event) override {
		m_Camera.OnEvent(event);
	}

	void Update(double dt) override {
		glm::vec3 direction(0.0f);

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
