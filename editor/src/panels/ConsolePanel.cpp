#include <imgui.h>

#include "panels/ConsolePanel.h"

#include "origo/core/Logger.h"

namespace OrigoEditor {

void ConsolePanel::on_im_gui_render() {
	draw_toolbar();
	ImGui::Separator();
	draw_log_view();
}

void ConsolePanel::draw_toolbar() {
	auto buffer = Origo::Logger::GetConsoleBuffer();

	if (ImGui::Button("Clear") && buffer) {
		buffer->Clear();
	}

	ImGui::SameLine();
	ImGui::Checkbox("Auto-scroll", &m_auto_scroll);
}

void ConsolePanel::draw_log_view() {
	auto buffer = Origo::Logger::GetConsoleBuffer();
	if (!buffer)
		return;

	auto entries = buffer->Copy();

	ImGui::BeginChild(
	    "ConsoleLogRegion",
	    ImVec2(0, 0),
	    false,
	    ImGuiWindowFlags_HorizontalScrollbar);

	bool stick_to_bottom = false;
	if (m_auto_scroll) {
		stick_to_bottom = (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f);
	}

	ImGui::PushTextWrapPos(0.0f);

	for (const auto& entry : entries) {
		const ImVec4 color = level_to_color(entry.level);

		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(level_to_string(entry.level));
		ImGui::SameLine();
		ImGui::TextUnformatted(entry.message.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::PopTextWrapPos();

	if (stick_to_bottom) {
		ImGui::SetScrollY(ImGui::GetScrollMaxY());
	}

	ImGui::EndChild();
}

ImVec4 ConsolePanel::level_to_color(spdlog::level::level_enum level) {
	switch (level) {
	case spdlog::level::trace:
		return { 0.60f, 0.60f, 0.60f, 1.0f };
	case spdlog::level::debug:
		return { 0.40f, 0.70f, 1.00f, 1.0f };
	case spdlog::level::info:
		return { 1.00f, 1.00f, 1.00f, 1.0f };
	case spdlog::level::warn:
		return { 1.00f, 0.80f, 0.30f, 1.0f };
	case spdlog::level::err:
		return { 1.00f, 0.40f, 0.40f, 1.0f };
	case spdlog::level::critical:
		return { 1.00f, 0.00f, 0.00f, 1.0f };
	default:
		return { 1.00f, 1.00f, 1.00f, 1.0f };
	}
}

const char* ConsolePanel::level_to_string(spdlog::level::level_enum level) {
	switch (level) {
	case spdlog::level::trace:
		return "[TRACE]";
	case spdlog::level::debug:
		return "[DEBUG]";
	case spdlog::level::info:
		return "[INFO ]";
	case spdlog::level::warn:
		return "[WARN]";
	case spdlog::level::err:
		return "[ERROR]";
	case spdlog::level::critical:
		return "[FATAL]";
	default:
		return "[?????]";
	}
}

}
