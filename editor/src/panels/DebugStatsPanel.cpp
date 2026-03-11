#include <imgui.h>

#include "panels/DebugStatsPanel.h"

namespace OrigoEditor {

static void draw_key_value(const char* key, const char* value) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::TextUnformatted(key);
	ImGui::TableSetColumnIndex(1);
	ImGui::TextUnformatted(value);
}

static void draw_key_value(const char* key, float value, const char* fmt = "%.2f") {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::TextUnformatted(key);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text(fmt, value);
}

static void draw_key_value(const char* key, uint32_t value) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::TextUnformatted(key);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%u", value);
}

void DebugStatsPanel::on_im_gui_render() {
	const ImVec4 green(0.2f, 0.8f, 0.2f, 1.0f);
	const ImVec4 red(0.8f, 0.2f, 0.2f, 1.0f);
	const ImVec4 yellow(0.9f, 0.7f, 0.2f, 1.0f);

	float dt = m_context.DeltaTime;
	float fps = dt > 0.0f ? 1.0f / dt : 0.0f;

	auto& scene_fb = m_context.RenderBuffer.get_specification();
	auto& game_fb = m_context.GameRenderBuffer.get_specification();

	if (ImGui::CollapsingHeader("Runtime", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginTable("RuntimeTable", 2, ImGuiTableFlags_SizingStretchProp)) {
			draw_key_value("FPS", glm::round(fps), "%.1f");
			draw_key_value("Delta Time (ms)", glm::round(dt * 1000.0f));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Panels");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted("Scene + Game");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Runtime State");
			ImGui::TableSetColumnIndex(1);

			switch (m_context.RuntimeState) {
			case EditorRuntimeState::Editing:
				ImGui::TextColored(yellow, "Editing");
				break;
			case EditorRuntimeState::Running:
				ImGui::TextColored(green, "Playing");
				break;
			}

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginTable("SceneTable", 2, ImGuiTableFlags_SizingStretchProp)) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Editor Scene");
			ImGui::TableSetColumnIndex(1);

			if (m_context.get_editor_scene())
				ImGui::TextColored(green, "Valid");
			else
				ImGui::TextColored(red, "None");

			draw_key_value(
			    "Editor Entity Count",
			    m_context.get_editor_scene() ? (uint32_t)m_context.get_editor_scene()->get_entities().size() : 0);
			draw_key_value(
			    "Runtime Entity Count",
			    m_context.get_runtime_scene() ? (uint32_t)m_context.get_runtime_scene()->get_entities().size() : 0);

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Viewport", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginTable("ViewportTable", 2, ImGuiTableFlags_SizingStretchProp)) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Scene Size");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u x %u", scene_fb.Width, scene_fb.Height);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Game Size");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u x %u", game_fb.Width, game_fb.Height);

			ImGui::EndTable();
		}
	}
}
}
