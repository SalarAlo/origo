#include "origo/assets/MaterialLibrary.h"
#include "origo/assets/ShaderLibrary.h"
#include "origo/assets/TextureCache.h"
#include "origo/assets/ShapeLibrary.h"
#include "origo/assets/ModelLibrary.h"

#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"

#include "origo/input/Input.h"

#include "origo/scene/MeshRenderer.h"

class GameApplication : public Origo::Application {
public:
	GameApplication(const Origo::ApplicationSettings& applicationSettings)
	    : Origo::Application(applicationSettings)
	    , m_Camera(m_Scene.GetCamera()) {
	}

	void Awake() override {
		m_Camera.SetSpeed(100);

		auto logoTexture { Origo::TextureCache::Load("resources/textures/rowlett.jpg") };

		m_Shader = Origo::ShaderLibrary::Load("normal");
		m_Material = Origo::MaterialLibrary::Create(m_Shader, logoTexture);

		SpawnTestGrid();
	}

	void SpawnTestGrid() {
		auto cubeMesh = Origo::ShapeLibrary::Load(Origo::ShapeLibrary::PrimitiveShape::CUBE);

		for (int i {}; i < GRID_SIZE; i++) {
			for (int j {}; j < GRID_SIZE; j++) {
				auto entity = m_Scene.CreateEntity();

				auto transform { entity->AttachComponent<Origo::Transform>() };
				transform->SetPosition(glm::vec3 { i * 20, j * 20, 11 });
				transform->SetScale(glm::vec3 { 10 });

				entity->AttachComponent<Origo::MeshRenderer>(m_Material, cubeMesh);
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
