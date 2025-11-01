#include "imgui_internal.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/PrimitiveShape.h"
#include "origo/assets/Shader.h"
#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"
#include "origo/events/Event.h"
#include "origo/events/KeyEvent.h"
#include "origo/events/WindowEvent.h"
#include "origo/input/Input.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"
#include "origo/scene/Scene.h"
#include "imgui.h"

class EditorApplication : public Origo::Application {
public:
	EditorApplication(const Origo::ApplicationSettings& settings)
	    : Origo::Application(settings)
	    , m_Camera(m_Scene.GetMainCamera())
	    , m_Buffer(Origo::MakeRef<Origo::FrameBuffer>(1600, 900)) { }

	void OnAwake() override {
		m_Camera->SetSpeed(10);

		auto logoTexture = Origo::AssetManager::CreateAsset<Origo::Texture>(
		    "Rowlett", "resources/textures/rowlett.jpg");
		m_Shader = Origo::AssetManager::CreateAsset<Origo::Shader>("Normal Shader", "normal");
		m_Material = Origo::AssetManager::CreateAsset<Origo::Material>("Normal Material", m_Shader, logoTexture);

		SpawnTestGrid();
	}

	void OnHandleEvent(Origo::Event& event) override {
		m_Camera->OnEvent(event);
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

		dispatcher.Dispatch<Origo::WindowResizeEvent>([&](Origo::WindowResizeEvent& e) {
			const auto size = e.GetSize();
			m_Buffer->Resize((int)size.x, (int)size.y);
		});
	}

	void OnRender() override {
		m_Buffer->Bind();

		glViewport(0, 0, m_Buffer->GetWidth(), m_Buffer->GetHeight());
		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Origo::Renderer::Flush(m_Scene.GetMainCamera());

		m_Buffer->Unbind();
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
		static bool opt_fullscreen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		static Origo::Entity* entity { nullptr };

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		ImGui::Begin("DockspaceHost", &dockspaceOpen, window_flags);
		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		// === Scene Viewport ===
		ImGui::Begin("Scene Viewport");
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (size.x > 0 && size.y > 0 && (m_Buffer->GetWidth() != (int)size.x || m_Buffer->GetHeight() != (int)size.y))
			m_Buffer->Resize((int)size.x, (int)size.y);

		ImGui::Image((ImTextureID)(uintptr_t)m_Buffer->GetColorTexHandle(), size, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();

		// === Hierarchy ===
		ImGui::Begin("Hierarchy");
		ImGui::Text("Scene Entities:");
		for (const auto& e : m_Scene.GetAllComponentsOfType<Origo::Transform>()) {
			bool selected = (entity && entity == e->AttachedTo.get());
			if (ImGui::Selectable(e->AttachedTo->GetName().c_str(), selected))
				entity = e->AttachedTo.get();
		}
		ImGui::End();

		// === Inspector ===
		ImGui::Begin("Inspector");
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

		if (!entity) {
			ImGui::TextDisabled("No entity selected.");
			ImGui::PopStyleVar();
		} else {
			ImGui::Text("Entity: %s", entity->GetName().c_str());
			ImGui::Separator();

			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				auto transform = m_Scene.GetComponent<Origo::Transform>(entity->GetId());
				glm::vec3 pos = transform->GetPosition();
				glm::vec3 rot = transform->GetRotation();
				glm::vec3 scl = transform->GetScale();

				ImGui::Spacing();
				DrawVec3Control("Position", pos);
				DrawVec3Control("Rotation", rot);
				DrawVec3Control("Scale", scl, 1.0f);
				ImGui::Spacing();

				transform->SetPosition(pos);
				transform->SetRotation(rot);
				transform->SetScale(scl);
			}

			ImGui::PopStyleVar();
		}

		ImGui::End();

		// === Assets ===
		ImGui::Begin("Asset Manager");
		ImGui::Text("Asset manager placeholder");
		ImGui::End();

		// === Console ===
		ImGui::Begin("Console");
		ImGui::Text("Console output placeholder");
		ImGui::End();

		ImGui::End(); // DockspaceHost
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 0, 0 });

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		// X
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 { 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 { 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 { 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 { 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Z
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 { 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 { 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

private:
	static constexpr int GRID_SIZE { 10 };
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

	Origo::Ref<Origo::Camera> m_Camera;
	Origo::Ref<Origo::Shader> m_Shader;
	Origo::Ref<Origo::Material> m_Material;
	Origo::Ref<Origo::FrameBuffer> m_Buffer;
};

Origo::Application* Origo::CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./editor",
		.WindowSettings = { .Width = 1800, .Height = 1000, .Title = "ITSAFLOATER" }
	};
	return new EditorApplication(settings);
}
