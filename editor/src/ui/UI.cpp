#include "ui/UI.h"
#include "imgui.h"
#include "origo/assets/AssetDatabase.h"
#include "panels/PanelManager.h"

namespace OrigoEditor::UI {

struct EditorPalette {
	ImVec4 bg { 0.11f, 0.11f, 0.11f, 1.00f };
	ImVec4 bgWindow { 0.14f, 0.14f, 0.14f, 1.00f };
	ImVec4 bgFrame { 0.18f, 0.18f, 0.18f, 1.00f };
	ImVec4 bgHover { 0.24f, 0.24f, 0.24f, 1.00f };
	ImVec4 bgActive { 0.30f, 0.30f, 0.30f, 1.00f };

	ImVec4 accent { 0.45f, 0.55f, 0.65f, 1.00f };

	ImVec4 text { 0.92f, 0.92f, 0.92f, 1.00f };
	ImVec4 textDim { 0.60f, 0.60f, 0.60f, 1.00f };

	ImVec4 border { 0.05f, 0.05f, 0.05f, 1.00f };
};

void ApplyEditorStyle() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* c = style.Colors;

	static const EditorPalette p;

	c[ImGuiCol_WindowBg] = p.bg;
	c[ImGuiCol_ChildBg] = p.bgWindow;
	c[ImGuiCol_PopupBg] = p.bgWindow;

	c[ImGuiCol_Border] = p.border;
	c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

	c[ImGuiCol_FrameBg] = p.bgFrame;
	c[ImGuiCol_FrameBgHovered] = p.bgHover;
	c[ImGuiCol_FrameBgActive] = p.bgActive;

	c[ImGuiCol_TitleBg] = p.bgFrame;
	c[ImGuiCol_TitleBgActive] = p.bgFrame;
	c[ImGuiCol_MenuBarBg] = p.bgFrame;

	c[ImGuiCol_Header] = p.bgFrame;
	c[ImGuiCol_HeaderHovered] = p.bgHover;
	c[ImGuiCol_HeaderActive] = p.bgActive;

	c[ImGuiCol_Button] = p.bgFrame;
	c[ImGuiCol_ButtonHovered] = p.bgHover;
	c[ImGuiCol_ButtonActive] = p.bgActive;

	c[ImGuiCol_Tab] = p.bgFrame;
	c[ImGuiCol_TabHovered] = p.bgHover;
	c[ImGuiCol_TabActive] = p.bgActive;
	c[ImGuiCol_TabUnfocused] = p.bg;
	c[ImGuiCol_TabUnfocusedActive] = p.bgFrame;

	c[ImGuiCol_Text] = p.text;
	c[ImGuiCol_TextDisabled] = p.textDim;

	c[ImGuiCol_Separator] = p.border;
	c[ImGuiCol_SeparatorHovered] = p.bgHover;
	c[ImGuiCol_SeparatorActive] = p.bgActive;

	c[ImGuiCol_ScrollbarBg] = p.bg;
	c[ImGuiCol_ScrollbarGrab] = p.bgFrame;
	c[ImGuiCol_ScrollbarGrabHovered] = p.bgHover;
	c[ImGuiCol_ScrollbarGrabActive] = p.bgActive;

	c[ImGuiCol_SliderGrab] = p.bgActive;
	c[ImGuiCol_SliderGrabActive] = p.bgHover;

	c[ImGuiCol_CheckMark] = p.accent;
	c[ImGuiCol_ResizeGrip] = p.bgFrame;
	c[ImGuiCol_ResizeGripHovered] = p.bgHover;
	c[ImGuiCol_ResizeGripActive] = p.bgActive;

	style.WindowRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 0.0f;

	style.FrameBorderSize = 1.0f;
	style.WindowBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;

	style.FramePadding = { 6, 4 };
	style.ItemSpacing = { 6, 6 };
	style.WindowPadding = { 10, 8 };
	style.IndentSpacing = 12.0f;

	style.WindowMenuButtonPosition = ImGuiDir_None;
}

void LoadEditorFont() {
	ImGuiIO& io = ImGui::GetIO();

	io.FontDefault = io.Fonts->AddFontFromFileTTF("resources/fonts/Inter.ttf", 16.0f);
	io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-Regular.ttf", 15.0f);

	IM_ASSERT(io.FontDefault);
}

void BeginDockspace() {
	static bool dockspaceOpen = true;
	static bool fullscreen = true;
	static ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	if (fullscreen) {
		const ImGuiViewport* vp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(vp->WorkPos);
		ImGui::SetNextWindowSize(vp->WorkSize);
		ImGui::SetNextWindowViewport(vp->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin("DockspaceHost", &dockspaceOpen, flags);
	ImGui::PopStyleVar();

	if (fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiID dockID = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(dockID, { 0, 0 }, dockFlags);
}

void EndDockspace() {
	ImGui::End();
}

void DrawMenuBar(PanelManager& manager) {
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu("File")) {
		ImGui::MenuItem("New Scene");
		ImGui::MenuItem("Open Scene...");
		ImGui::MenuItem("Save Scene");
		if (ImGui::MenuItem("Save Generated Assets")) {
			Origo::AssetDatabase::SaveAll();
		}

		ImGui::Separator();
		ImGui::EndMenu();
	}

	manager.RenderMenuItems();
	ImGui::EndMenuBar();
}

}
