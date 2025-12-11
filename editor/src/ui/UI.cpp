#include "ui/UI.h"
#include "imgui.h"
#include "origo/assets/AssetDatabase.h"
#include "panels/PanelManager.h"

namespace OrigoEditor::UI {

struct EditorPalette {
	ImVec4 bg { 0.090f, 0.090f, 0.090f, 1.00f };
	ImVec4 bgWindow { 0.115f, 0.115f, 0.115f, 1.00f };
	ImVec4 bgFrame { 0.145f, 0.145f, 0.145f, 1.00f };
	ImVec4 bgHover { 0.195f, 0.195f, 0.195f, 1.00f };
	ImVec4 bgActive { 0.250f, 0.250f, 0.250f, 1.00f };

	ImVec4 dockBg { 0.090f, 0.090f, 0.090f, 1.00f };

	ImVec4 accent { 0.22f, 0.45f, 0.78f, 1.00f };
	ImVec4 accentSoft { 0.18f, 0.34f, 0.55f, 1.00f };
	ImVec4 accentError { 0.82f, 0.25f, 0.25f, 1.00f };
	ImVec4 selection { 0.18f, 0.34f, 0.55f, 0.35f };

	ImVec4 text { 0.88f, 0.88f, 0.88f, 1.00f };
	ImVec4 textDim { 0.50f, 0.50f, 0.50f, 1.00f };

	ImVec4 border { 0.06f, 0.06f, 0.06f, 1.00f };
	ImVec4 borderStrong { 0.12f, 0.12f, 0.12f, 1.00f };
};

void ApplyEditorStyle() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* c = style.Colors;

	static const EditorPalette p;

	c[ImGuiCol_WindowBg] = p.bgWindow;
	c[ImGuiCol_ChildBg] = p.bg;
	c[ImGuiCol_PopupBg] = p.bgWindow;

	c[ImGuiCol_Border] = p.border;
	c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

	c[ImGuiCol_FrameBg] = p.bgFrame;
	c[ImGuiCol_FrameBgHovered] = p.bgHover;
	c[ImGuiCol_FrameBgActive] = p.bgActive;

	c[ImGuiCol_TitleBg] = p.bgFrame;
	c[ImGuiCol_TitleBgActive] = p.bgFrame;
	c[ImGuiCol_TitleBgCollapsed] = p.bg;
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

	c[ImGuiCol_Separator] = p.borderStrong;
	c[ImGuiCol_SeparatorHovered] = p.bgHover;
	c[ImGuiCol_SeparatorActive] = p.bgActive;

	c[ImGuiCol_ScrollbarBg] = p.bg;
	c[ImGuiCol_ScrollbarGrab] = p.bgFrame;
	c[ImGuiCol_ScrollbarGrabHovered] = p.bgHover;
	c[ImGuiCol_ScrollbarGrabActive] = p.bgActive;

	c[ImGuiCol_SliderGrab] = p.accentSoft;
	c[ImGuiCol_SliderGrabActive] = p.accent;

	c[ImGuiCol_CheckMark] = p.accent;
	c[ImGuiCol_ResizeGrip] = p.bgFrame;
	c[ImGuiCol_ResizeGripHovered] = p.bgHover;
	c[ImGuiCol_ResizeGripActive] = p.bgActive;

	c[ImGuiCol_TextSelectedBg] = p.selection;
	c[ImGuiCol_DockingPreview] = p.accentSoft;
	c[ImGuiCol_DockingEmptyBg] = p.dockBg;

	c[ImGuiCol_TableHeaderBg] = p.bgFrame;
	c[ImGuiCol_TableBorderStrong] = p.borderStrong;
	c[ImGuiCol_TableBorderLight] = p.border;
	c[ImGuiCol_TableRowBg] = ImVec4(p.bgWindow.x, p.bgWindow.y, p.bgWindow.z, 1.00f);
	c[ImGuiCol_TableRowBgAlt] = ImVec4(p.bgFrame.x, p.bgFrame.y, p.bgFrame.z, 1.00f);

	c[ImGuiCol_NavHighlight] = p.accentSoft;
	c[ImGuiCol_NavWindowingHighlight] = ImVec4(p.accent.x, p.accent.y, p.accent.z, 0.40f);
	c[ImGuiCol_NavWindowingDimBg] = ImVec4(0, 0, 0, 0.35f);
	c[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.50f);

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
		if (ImGui::MenuItem("Save Generated Assets"))
			Origo::AssetDatabase::SaveAll();
		ImGui::Separator();
		ImGui::EndMenu();
	}

	manager.RenderMenuItems();
	ImGui::EndMenuBar();
}

}
