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
#include "imgui.h"

#include "panels/AssetsPanel.h"
#include "panels/ConsolePanel.h"
#include "panels/HierarchyPanel.h"
#include "panels/SceneViewport.h"
#include "panels/InspectorPanel.h"

class EditorApplication : public Origo::Application {
public:
	EditorApplication(const Origo::ApplicationSettings& settings)
	    : Origo::Application(settings)
	    , m_Camera(m_Scene.GetMainCamera())
	    , m_Buffer(1600, 900)
	    , m_Viewport(m_Buffer, m_Camera)
	    , m_Hierachy(m_Scene)
	    , m_Inspector(m_Scene) {
		m_Panels.push_back(&m_Viewport);
		m_Panels.push_back(&m_Hierachy);
		m_Panels.push_back(&m_Inspector);
		m_Panels.push_back(&m_Assets);
		m_Panels.push_back(&m_Console);
	}

	void OnAwake() override {
		m_Camera->SetSpeed(10);

		Origo::Input::SetCursorMode(Origo::Input::CursorMode::Free);
		m_Camera->SetSensitivity(0);

		auto logoTexture = Origo::AssetManager::CreateAsset<Origo::Texture>("Rowlett", "resources/textures/rowlett.jpg");
		m_Shader = Origo::AssetManager::CreateAsset<Origo::Shader>("Normal Shader", "normal");
		m_Material = Origo::AssetManager::CreateAsset<Origo::Material>("Normal Material", m_Shader, logoTexture);

		SpawnTestGrid();
	}

	void OnHandleEvent(Origo::Event& event) override {
		Origo::EventDispatcher dispatcher { event };

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

	void OnRender() override {
		m_Buffer.Bind();

		Origo::Renderer::Clear(m_Buffer.GetWidth(), m_Buffer.GetHeight());
		Origo::Renderer::Flush(m_Camera);

		m_Buffer.Unbind();
	}

	void OnUpdate(double dt) override {
		glm::vec3 direction(0.0f);
		constexpr float normalSpeed { 10 };
		constexpr float fastSpeed { normalSpeed * 5 };

		m_Camera->SetSpeed(Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_LEFT_SHIFT)
		        ? fastSpeed
		        : normalSpeed);

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

	void OnImGuiRender() override {
		static bool dockspaceOpen = true;
		static bool fullscreen = true;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (fullscreen) {
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		ImGui::Begin("DockspaceHost", &dockspaceOpen, windowFlags);

		if (fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiID dockspaceId = ImGui::GetID("MainDockspace");
		ImGui::DockSpace(dockspaceId, ImVec2(0, 0), dockspaceFlags);

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Scene")) {
				}
				if (ImGui::MenuItem("Open Scene...")) {
				}
				if (ImGui::MenuItem("Save Scene")) {
				}

				ImGui::Separator();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {
				for (const auto& panel : m_Panels) {
					ImGui::MenuItem(panel->GetName(), nullptr, &panel->IsOpenRef());
				}
			}
			ImGui::EndMenuBar();
		}

		m_Inspector.SetSelectedEntity(m_Hierachy.GetSelectedEntity());

		for (const auto& panel : m_Panels) {
			if (panel->IsOpen()) {
				ImGui::Begin(panel->GetName());
				panel->OnImGuiRender();
				ImGui::End();
			}
		}

		ImGui::End();
	}

private:
	void SpawnTestGrid() {
		auto cubeMesh = Origo::AssetManager::CreateAsset<Origo::Mesh>("Cube", Origo::PrimitiveShape::Cube);
		for (int i = 0; i < GRID_SIZE; ++i) {
			for (int j = 0; j < GRID_SIZE; ++j) {
				auto entity = m_Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
				auto transform = m_Scene.AddComponent<Origo::Transform>(entity);
				transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });
				m_Scene.AddComponent<Origo::MeshRenderer>(entity, m_Material, cubeMesh);
			}
		}
	}

private:
	static constexpr int GRID_SIZE { 10 };

	Origo::Ref<Origo::Camera> m_Camera;
	Origo::Ref<Origo::Shader> m_Shader;
	Origo::Ref<Origo::Material> m_Material;
	Origo::FrameBuffer m_Buffer;

	OrigoEditor::SceneViewport m_Viewport { m_Buffer, m_Camera };
	OrigoEditor::HierarchyPanel m_Hierachy { m_Scene };
	OrigoEditor::InspectorPanel m_Inspector { m_Scene };
	OrigoEditor::ConsolePanel m_Console {};
	OrigoEditor::AssetsPanel m_Assets {};

	std::vector<OrigoEditor::EditorPanel*> m_Panels;
};

Origo::Application* Origo::CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./editor",
		.WindowSettings = { .Width = 1800, .Height = 1000, .Title = "ITSAFLOATER" }
	};
	return new EditorApplication(settings);
}
