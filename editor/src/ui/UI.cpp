#include "ui/UI.h"
#include "imgui.h"
#include "panels/PanelManager.h"
#include <string>

namespace OrigoEditor::UI {

void ApplyEditorStyle() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	const ImVec4 bg = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
	const ImVec4 bgLight = ImVec4(0.14f, 0.14f, 0.15f, 1.00f);
	const ImVec4 bgHover = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
	const ImVec4 bgActive = ImVec4(0.26f, 0.27f, 0.29f, 1.00f);
	const ImVec4 accent = ImVec4(0.33f, 0.53f, 0.98f, 1.00f);
	const ImVec4 text = ImVec4(0.95f, 0.96f, 0.97f, 1.00f);
	const ImVec4 textDim = ImVec4(0.65f, 0.67f, 0.70f, 1.00f);

	colors[ImGuiCol_WindowBg] = bg;
	colors[ImGuiCol_ChildBg] = bg;
	colors[ImGuiCol_PopupBg] = bgLight;
	colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0.25f);

	colors[ImGuiCol_FrameBg] = bgLight;
	colors[ImGuiCol_FrameBgHovered] = bgHover;
	colors[ImGuiCol_FrameBgActive] = bgActive;

	colors[ImGuiCol_Header] = bgLight;
	colors[ImGuiCol_HeaderHovered] = bgHover;
	colors[ImGuiCol_HeaderActive] = bgActive;
	colors[ImGuiCol_Button] = bgLight;
	colors[ImGuiCol_ButtonHovered] = accent;
	colors[ImGuiCol_ButtonActive] = ImVec4(accent.x * 0.8f, accent.y * 0.8f, accent.z * 0.8f, 1.0f);

	colors[ImGuiCol_Tab] = bgLight;
	colors[ImGuiCol_TabHovered] = accent;
	colors[ImGuiCol_TabActive] = bgActive;
	colors[ImGuiCol_TabUnfocused] = bg;
	colors[ImGuiCol_TabUnfocusedActive] = bgLight;

	colors[ImGuiCol_TitleBg] = bg;
	colors[ImGuiCol_TitleBgActive] = bgLight;
	colors[ImGuiCol_MenuBarBg] = bgLight;

	colors[ImGuiCol_Text] = text;
	colors[ImGuiCol_TextDisabled] = textDim;

	colors[ImGuiCol_SliderGrab] = accent;
	colors[ImGuiCol_SliderGrabActive] = ImVec4(accent.x * 0.9f, accent.y * 0.9f, accent.z * 0.9f, 1.0f);

	colors[ImGuiCol_ScrollbarBg] = bg;
	colors[ImGuiCol_ScrollbarGrab] = bgLight;
	colors[ImGuiCol_ScrollbarGrabHovered] = bgHover;
	colors[ImGuiCol_ScrollbarGrabActive] = bgActive;

	style.WindowRounding = 6.0f;
	style.ChildRounding = 5.0f;
	style.FrameRounding = 5.0f;
	style.GrabRounding = 3.0f;
	style.ScrollbarRounding = 4.0f;
	style.TabRounding = 5.0f;

	style.FramePadding = ImVec2(8, 5);
	style.ItemSpacing = ImVec2(10, 8);
	style.WindowPadding = ImVec2(12, 10);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.IndentSpacing = 20.0f;

	style.WindowBorderSize = 1.0f;
	style.FrameBorderSize = 0.0f;
	style.PopupBorderSize = 1.0f;
}

void LoadEditorFont() {
	ImGuiIO& io = ImGui::GetIO();
	const std::string fontPath = "resources/fonts/Inter.ttf";

	io.FontDefault = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 19.5f);
	IM_ASSERT(io.FontDefault && "Failed to load Inter.ttf!");
}

void BeginDockspace() {
	static bool dockspaceOpen = true;
	static bool fullscreen = true;
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

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

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("DockspaceHost", &dockspaceOpen, windowFlags);
	ImGui::PopStyleVar();

	if (fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiID dockspaceId = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(dockspaceId, ImVec2(0, 0), dockspaceFlags);
}

void EndDockspace() {
	ImGui::End();
}

void DrawMenuBar(PanelManager& manager) {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("New Scene");
			ImGui::MenuItem("Open Scene...");
			ImGui::MenuItem("Save Scene");
			ImGui::Separator();
			ImGui::EndMenu();
		}

		manager.RenderMenuItems();
		ImGui::EndMenuBar();
	}
}

}
