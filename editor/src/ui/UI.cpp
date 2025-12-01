#include "ui/UI.h"
#include "imgui.h"
#include "panels/PanelManager.h"

namespace OrigoEditor::UI {

struct EditorPalette {
	ImVec4 bg { 0.07f, 0.07f, 0.08f, 1.00f };
	ImVec4 bgWindow { 0.10f, 0.10f, 0.11f, 1.00f };
	ImVec4 bgFrame { 0.15f, 0.15f, 0.17f, 1.00f };
	ImVec4 bgHover { 0.20f, 0.21f, 0.23f, 1.00f };
	ImVec4 bgActive { 0.27f, 0.28f, 0.30f, 1.00f };

	ImVec4 accent { 0.32f, 0.52f, 0.95f, 1.00f };

	ImVec4 accentGood { 0.32f, 0.70f, 0.45f, 1.00f };
	ImVec4 accentWarn { 0.90f, 0.55f, 0.22f, 1.00f };

	ImVec4 text { 0.95f, 0.96f, 0.97f, 1.00f };
	ImVec4 textDim { 0.60f, 0.62f, 0.65f, 1.00f };

	ImVec4 border { 0.00f, 0.00f, 0.00f, 0.60f };
};

inline ImVec4 Darken(const ImVec4& c, float factor = 0.85f) {
	return { c.x * factor, c.y * factor, c.z * factor, c.w };
}

void ApplyEditorStyle() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* c = style.Colors;

	static const EditorPalette p;

	c[ImGuiCol_WindowBg] = p.bgWindow;
	c[ImGuiCol_ChildBg] = p.bgWindow;
	c[ImGuiCol_PopupBg] = p.bgFrame;
	c[ImGuiCol_Border] = p.border;

	c[ImGuiCol_Header] = p.bgFrame;
	c[ImGuiCol_HeaderHovered] = p.bgHover;
	c[ImGuiCol_HeaderActive] = p.bgActive;

	c[ImGuiCol_Button] = p.bgFrame;
	c[ImGuiCol_ButtonHovered] = p.accent;
	c[ImGuiCol_ButtonActive] = ImVec4(
	    p.accent.x * 0.85f,
	    p.accent.y * 0.85f,
	    p.accent.z * 0.85f,
	    1.0f);

	c[ImGuiCol_Tab] = p.bgFrame;
	c[ImGuiCol_TabHovered] = p.accent;
	c[ImGuiCol_TabActive] = p.bgActive;
	c[ImGuiCol_TabUnfocused] = p.bg;
	c[ImGuiCol_TabUnfocusedActive] = p.bgFrame;

	c[ImGuiCol_TitleBg] = p.bg;
	c[ImGuiCol_TitleBgActive] = p.bgFrame;
	c[ImGuiCol_MenuBarBg] = p.bgFrame;

	c[ImGuiCol_Text] = p.text;
	c[ImGuiCol_TextDisabled] = p.textDim;

	c[ImGuiCol_SliderGrab] = p.accent;
	c[ImGuiCol_SliderGrabActive] = ImVec4(
	    p.accent.x * 0.80f,
	    p.accent.y * 0.80f,
	    p.accent.z * 0.80f,
	    1.0f);

	c[ImGuiCol_ScrollbarBg] = p.bgWindow;
	c[ImGuiCol_ScrollbarGrab] = p.bgFrame;
	c[ImGuiCol_ScrollbarGrabHovered] = p.bgHover;
	c[ImGuiCol_ScrollbarGrabActive] = p.bgActive;

	style.ChildRounding = 5.0f;
	style.FrameRounding = 4.0f;
	style.GrabRounding = 3.0f;
	style.ScrollbarRounding = 4.0f;
	style.TabRounding = 4.0f;

	style.FrameBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.WindowBorderSize = 1.0f;

	style.FramePadding = { 7, 5 };
	style.ItemSpacing = { 10, 7 };
	style.WindowPadding = { 12, 10 };
	style.IndentSpacing = 18.0f;

	style.WindowMenuButtonPosition = ImGuiDir_None;
}

void LoadEditorFont() {
	ImGuiIO& io = ImGui::GetIO();

	io.FontDefault = io.Fonts->AddFontFromFileTTF("resources/fonts/Inter.ttf", 16.0f);
	io.Fonts->AddFontFromFileTTF("resources/fonts/JetBrainsMono-Regular.ttf", 15.0f);

	IM_ASSERT(io.FontDefault && "Failed to load Inter.ttf!");
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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

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
		ImGui::Separator();
		ImGui::EndMenu();
	}

	manager.RenderMenuItems();
	ImGui::EndMenuBar();
}
}
