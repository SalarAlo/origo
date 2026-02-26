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

	auto& fb = m_context.RenderBuffer.get_specification();

	if (ImGui::CollapsingHeader("Runtime", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginTable("RuntimeTable", 2, ImGuiTableFlags_SizingStretchProp)) {
			draw_key_value("FPS", glm::round(fps), "%.1f");
			draw_key_value("Delta Time (ms)", glm::round(dt * 1000.0f));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("View Mode");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(
			    m_context.ViewMode == EditorViewMode::Editor ? "Editor" : "Runtime");

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
			ImGui::Text("Active Scene");
			ImGui::TableSetColumnIndex(1);

			if (m_context.ActiveScene)
				ImGui::TextColored(green, "Valid");
			else
				ImGui::TextColored(red, "None");

			draw_key_value(
			    "Entity Count",
			    m_context.ActiveScene ? (uint32_t)m_context.ActiveScene->get_entities().size() : 0);

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Viewport", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginTable("ViewportTable", 2, ImGuiTableFlags_SizingStretchProp)) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Resolution");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u x %u", fb.Width, fb.Height);

			ImGui::EndTable();
		}
	}
}
}
