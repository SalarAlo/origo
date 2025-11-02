#include "origo/assets/AssetManager.h"
#include "origo/assets/PrimitiveShape.h"
#include "origo/assets/Shader.h"
#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"
#include "origo/events/Event.h"
#include "origo/events/KeyEvent.h"
#include "origo/input/Input.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"
#include "origo/scene/Scene.h"

#include "panels/AssetsPanel.h"
#include "panels/ConsolePanel.h"
#include "panels/HierarchyPanel.h"
#include "panels/PanelManager.h"
#include "panels/SceneViewport.h"
#include "panels/InspectorPanel.h"
#include "ui/UI.h"

namespace OrigoEditor {

using namespace Origo;

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationSettings& settings)
	    : Application(settings)
	    , m_Camera(m_Scene.GetMainCamera())
	    , m_Buffer(1600, 900) {
		m_HierachyId = m_PanelManager.AddPanel<HierarchyPanel>(m_Scene);
		m_InspectorId = m_PanelManager.AddPanel<InspectorPanel>(m_Scene);
		m_PanelManager.AddPanel<SceneViewport>(m_Buffer, m_Camera);
		m_PanelManager.AddPanel<AssetsPanel>();
		m_PanelManager.AddPanel<ConsolePanel>();
	}

	void OnAwake() override {
		m_Camera->SetSpeed(10);

		UI::ApplyEditorStyle();
		UI::LoadEditorFont();

		Input::SetCursorMode(Input::CursorMode::Free);
		m_Camera->SetSensitivity(0);

		auto logoTexture = AssetManager::CreateAsset<Texture>("Rowlett", "resources/textures/origo_logo.png");
		m_Shader = AssetManager::CreateAsset<Shader>("Normal Shader", "normal");
		m_Material = AssetManager::CreateAsset<Material>("Normal Material", m_Shader, logoTexture);

		SpawnTestGrid();
	}

	void OnHandleEvent(Event& event) override {
		EventDispatcher dispatcher { event };

		dispatcher.Dispatch<KeyPressEvent>([&](KeyPressEvent& e) {
			if (e.GetKeyPressed() == KeyboardKey::KEY_ESCAPE) {
				Input::SetCursorMode(Input::CursorMode::Free);
				m_Camera->SetSensitivity(0);
			} else if (e.GetKeyPressed() == KeyboardKey::KEY_BACKSPACE) {
				Input::SetCursorMode(Input::CursorMode::Locked);
				m_Camera->SetSensitivity(0.1);
			}
		});
	}

	void OnRender() override {
		m_Buffer.Bind();

		Renderer::Clear(m_Buffer.GetWidth(), m_Buffer.GetHeight());
		Renderer::Flush(m_Camera);

		m_Buffer.Unbind();
	}

	void OnUpdate(double dt) override {
		glm::vec3 direction(0.0f);
		constexpr float normalSpeed { 10 };
		constexpr float fastSpeed { normalSpeed * 5 };

		m_Camera->SetSpeed(Input::IsKeyPressed(KeyboardKey::KEY_LEFT_SHIFT)
		        ? fastSpeed
		        : normalSpeed);

		if (Input::IsKeyPressed(KeyboardKey::KEY_W))
			direction += m_Camera->GetForward();

		if (Input::IsKeyPressed(KeyboardKey::KEY_S))
			direction -= m_Camera->GetForward();

		if (Input::IsKeyPressed(KeyboardKey::KEY_D))
			direction += m_Camera->GetRight();

		if (Input::IsKeyPressed(KeyboardKey::KEY_A))
			direction -= m_Camera->GetRight();

		if (glm::length(direction) > 0.0f)
			m_Camera->Move(glm::normalize(direction) * static_cast<float>(dt));
	}

	void OnImGuiRender() override {
		UI::BeginDockspace();

		UI::DrawMenuBar(m_PanelManager);

		auto inspector { (static_cast<InspectorPanel*>(m_PanelManager.GetPanel(m_InspectorId))) };
		auto hierachy { (static_cast<HierarchyPanel*>(m_PanelManager.GetPanel(m_HierachyId))) };

		inspector->SetSelectedEntity(hierachy->GetSelectedEntity());

		m_PanelManager.RenderPanels();

		UI::EndDockspace();
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
	Ref<Shader> m_Shader;
	Ref<Material> m_Material;
	FrameBuffer m_Buffer;

	PanelManager m_PanelManager;

	int m_InspectorId {};
	int m_HierachyId {};
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
