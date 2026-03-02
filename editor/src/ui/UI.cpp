#include <nfd.h>

#include "ui/UI.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"

#include "origo/assets/serialization/SceneSerializer.h"

#include "origo/scripting/ScriptSystem.h"

#include "panels/PanelManager.h"

#include "systems/EditorRuntimeController.h"

namespace OrigoEditor::UI {

ImFont* ui_font = nullptr;
ImFont* code_font = nullptr;

void apply_editor_style(const EditorPalette& p) {
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* c = style.Colors;

	c[ImGuiCol_WindowBg] = p.Surface1;
	c[ImGuiCol_ChildBg] = p.Surface0;
	c[ImGuiCol_PopupBg] = p.Surface1;

	c[ImGuiCol_Text] = p.Text;
	c[ImGuiCol_TextDisabled] = p.TextDim;

	c[ImGuiCol_Border] = p.Border;
	c[ImGuiCol_BorderShadow] = { 0, 0, 0, 0 };

	c[ImGuiCol_FrameBg] = p.Surface2;
	c[ImGuiCol_FrameBgHovered] = p.Hover;
	c[ImGuiCol_FrameBgActive] = p.Active;

	c[ImGuiCol_TitleBg] = p.Surface0;
	c[ImGuiCol_TitleBgActive] = p.Surface1;
	c[ImGuiCol_TitleBgCollapsed] = p.Surface0;

	c[ImGuiCol_MenuBarBg] = p.Surface1;

	c[ImGuiCol_Header] = p.Surface2;
	c[ImGuiCol_HeaderHovered] = p.Hover;
	c[ImGuiCol_HeaderActive] = p.Active;

	c[ImGuiCol_Button] = p.Surface2;
	c[ImGuiCol_ButtonHovered] = p.Hover;
	c[ImGuiCol_ButtonActive] = p.Active;

	c[ImGuiCol_Tab] = p.Surface0;
	c[ImGuiCol_TabHovered] = p.Hover;
	c[ImGuiCol_TabActive] = p.Surface2;
	c[ImGuiCol_TabUnfocused] = p.Surface0;
	c[ImGuiCol_TabUnfocusedActive] = p.Surface1;

	c[ImGuiCol_Separator] = p.BorderStrong;
	c[ImGuiCol_SeparatorHovered] = p.AccentSoft;
	c[ImGuiCol_SeparatorActive] = p.Accent;

	c[ImGuiCol_ScrollbarBg] = p.Surface0;
	c[ImGuiCol_ScrollbarGrab] = p.Surface2;
	c[ImGuiCol_ScrollbarGrabHovered] = p.Hover;
	c[ImGuiCol_ScrollbarGrabActive] = p.Active;

	c[ImGuiCol_CheckMark] = p.Accent;
	c[ImGuiCol_SliderGrab] = p.AccentSoft;
	c[ImGuiCol_SliderGrabActive] = p.Accent;

	c[ImGuiCol_TextSelectedBg] = p.Selection;

	c[ImGuiCol_DockingPreview] = { p.Accent.x, p.Accent.y, p.Accent.z, 0.60f };
	c[ImGuiCol_DockingEmptyBg] = p.Surface0;

	c[ImGuiCol_TableHeaderBg] = p.Surface2;
	c[ImGuiCol_TableRowBg] = p.Surface1;
	c[ImGuiCol_TableRowBgAlt] = p.Surface0;
	c[ImGuiCol_TableBorderStrong] = p.BorderStrong;
	c[ImGuiCol_TableBorderLight] = p.Border;

	c[ImGuiCol_NavHighlight] = p.AccentSoft;
	c[ImGuiCol_NavWindowingHighlight] = { p.Accent.x, p.Accent.y, p.Accent.z, 0.40f };
	c[ImGuiCol_NavWindowingDimBg] = { 0, 0, 0, 0.35f };
	c[ImGuiCol_ModalWindowDimBg] = { 0, 0, 0, 0.50f };

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

void load_editor_font() {
	ImGuiIO& io = ImGui::GetIO();

	ui_font = io.Fonts->AddFontFromFileTTF(
	    "resources/fonts/Inter.ttf",
	    17.0f);

	code_font = io.Fonts->AddFontFromFileTTF(
	    "resources/fonts/JetBrainsMono-Regular.ttf",
	    17.0f);

	io.FontDefault = ui_font;

	IM_ASSERT(ui_font && code_font);
}

void begin_dockspace() {
	static bool dockspace_open = true;
	static bool fullscreen = true;
	static ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_PassthruCentralNode;

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
	ImGui::Begin("DockspaceHost", &dockspace_open, flags);
	ImGui::PopStyleVar();

	if (fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiID dock_id = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(dock_id, { 0, 0 }, dock_flags);
}

void end_dockspace() {
	ImGui::End();
}

void draw_menu_bar(PanelManager& manager, EditorContext& ctx) {
	static EditorRuntimeController controller { ctx };

	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu("File")) {

		ImGui::MenuItem("New Scene");

		if (ImGui::MenuItem("Open Scene...")) {

			nfdchar_t* out_path = nullptr;

			nfdfilteritem_t filters[] = {
				{ "Origo Scene", "scene.json" }
			};

			nfdresult_t result = NFD_OpenDialog(
			    &out_path,
			    filters,
			    1,
			    "./resources/scenes");

			if (result == NFD_OKAY) {
				std::filesystem::path scene_path = out_path;
				NFD_FreePath(out_path);

				if (std::filesystem::exists(scene_path)) {
					ctx.PendingScene = Origo::SceneSerializer::deserialize_from_file(scene_path);
				} else {
					ORG_CORE_ERROR("Scene file does not exist: {}", scene_path.string());
				}
			} else if (result == NFD_ERROR) {
				ORG_CORE_ERROR("NFD error: {}", NFD_GetError());
			}
		}

		if (ImGui::MenuItem("Save Scene")) {
			std::string path = "./resources/scenes/";
			path += ctx.EditorScene->get_name();
			path += ".scene.json";
			Origo::SceneSerializer::serialize_to_file(*ctx.EditorScene, path);
		}

		if (ImGui::MenuItem("Save Assets")) {
			Origo::AssetDatabase::get_instance().save_assets();
		}

		if (ImGui::MenuItem("Reload Scripts")) {
			Origo::AssetManager::get_instance().resolve_all(
			    [](Origo::Asset* a) {
				    return a->get_asset_type() == Origo::AssetType::Script;
			    });
			Origo::ScriptSystem::reload_all();
		}

		ImGui::Separator();
		ImGui::EndMenu();
	}

	manager.render_menu_items();
	ImGui::EndMenuBar();
}

}
