#include "panels/ConsolePanel.h"
#include "origo/core/Logger.h"

#include <imgui.h>

namespace OrigoEditor {

void ConsolePanel::OnImGuiRender() {
	ImGui::Begin("Console");

	DrawToolbar();
	ImGui::Separator();
	DrawLogView();

	ImGui::End();
}

void ConsolePanel::DrawToolbar() {
	auto buffer = Origo::Logger::GetConsoleBuffer();

	if (ImGui::Button("Clear") && buffer) {
		buffer->Clear();
	}

	ImGui::SameLine();
	ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
}

void ConsolePanel::DrawLogView() {
	auto buffer = Origo::Logger::GetConsoleBuffer();
	if (!buffer)
		return;

	auto entries = buffer->Copy();

	ImGui::BeginChild(
	    "ConsoleLogRegion",
	    ImVec2(0, 0),
	    false,
	    ImGuiWindowFlags_HorizontalScrollbar);

	bool stickToBottom = false;
	if (m_AutoScroll) {
		stickToBottom = (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f);
	}

	ImGui::PushTextWrapPos(0.0f);

	for (const auto& entry : entries) {
		const ImVec4 color = LevelToColor(entry.level);

		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(LevelToString(entry.level));
		ImGui::SameLine();
		ImGui::TextUnformatted(entry.message.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::PopTextWrapPos();

	if (stickToBottom) {
		ImGui::SetScrollY(ImGui::GetScrollMaxY());
	}

	ImGui::EndChild();
}

ImVec4 ConsolePanel::LevelToColor(spdlog::level::level_enum level) {
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

const char* ConsolePanel::LevelToString(spdlog::level::level_enum level) {
	switch (level) {
	case spdlog::level::trace:
		return "[TRACE]";
	case spdlog::level::debug:
		return "[DEBUG]";
	case spdlog::level::info:
		return "[INFO ]";
	case spdlog::level::warn:
		return "[WARN ]";
	case spdlog::level::err:
		return "[ERROR]";
	case spdlog::level::critical:
		return "[FATAL]";
	default:
		return "[?????]";
	}
}

}
