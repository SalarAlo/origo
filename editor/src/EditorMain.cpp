#include "controllers/EditorCameraController.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/PrimitiveShape.h"
#include "origo/assets/Shader.h"
#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"
#include "origo/events/Event.h"
#include "origo/input/Input.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/MeshRenderer.h"

#include "panels/PanelManager.h"
#include "controllers/EditorContext.h"
#include "controllers/EditorUIPanelController.h"
#include "ui/UI.h"

namespace OrigoEditor {

using namespace Origo;

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationSettings& settings)
	    : Application(settings)
	    , m_Camera(m_Scene.GetMainCamera())
	    , m_CameraController(m_Camera)
	    , m_Buffer(1600, 900)
	    , m_Context(m_Scene, m_Camera, m_Buffer, m_PanelManager)
	    , m_PanelController(m_Context) {
	}

	void OnAwake() override {

		UI::ApplyEditorStyle();
		UI::LoadEditorFont();

		Input::SetCursorMode(Input::CursorMode::Free);
		m_Camera->SetSensitivity(0);

		auto logoTexture = AssetManager::CreateAsset<Texture>("Rowlett", "resources/textures/origo_logo.png");
		m_Shader = AssetManager::CreateAsset<Shader>("Normal Shader", "normal");
		m_Material = AssetManager::CreateAsset<Material>("Normal Material", m_Shader, logoTexture);

		SpawnTestGrid();
	}

	void OnEvent(Event& event) override {
		m_CameraController.HandleEvent(event);
	}

	void OnUpdate(double dt) override {
		m_CameraController.Update(dt);

		m_Buffer.Bind();

		Renderer::SetViewport(m_Buffer.GetWidth(), m_Buffer.GetHeight());
		GLCall(glClearColor(0.1f, 0.1f, 0.12f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		Renderer::Flush(m_Camera);

		m_Buffer.Unbind();

		m_ImGuiLayer.Begin();

		m_PanelController.Render();

		m_ImGuiLayer.End();
	}

private:
	void SpawnTestGrid() {
		auto cubeMesh = AssetManager::CreateAsset<Mesh>("Cube", PrimitiveShape::Cube);

		for (int i = 0; i < GRID_SIZE; ++i) {
			for (int j = 0; j < GRID_SIZE; ++j) {
				auto entity = m_Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
				auto transform = m_Scene.AddComponent<Transform>(entity);
				transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });
				m_Scene.AddComponent<MeshRenderer>(entity, m_Material, cubeMesh);
			}
		}
	}

private:
	static constexpr int GRID_SIZE { 10 };

	Ref<Camera> m_Camera;
	FrameBuffer m_Buffer;
	PanelManager m_PanelManager;

	Ref<Shader> m_Shader;
	Ref<Material> m_Material;

	EditorContext m_Context;

	EditorCameraController m_CameraController;
	EditorUIPanelController m_PanelController;
};

}

namespace Origo {
Application* CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./",
		.WindowSettings = { .Width = 1800, .Height = 1000, .Title = "ITSAFLOATER" }
	};
	return new OrigoEditor::EditorApplication(settings);
}
}
