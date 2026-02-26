#include "panels/RuntimeStatePanel.h"

#include "state/EditorContext.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {

RuntimeStatePanel::RuntimeStatePanel(EditorContext& context)
    : m_context(context)
    , m_controller(context) {
}

void RuntimeStatePanel::on_im_gui_render() {
	const ImVec2 button_size { 24.0f, 24.0f };
	const auto& style = ImGui::GetStyle();

	ImTextureID play_icon = EditorIcons::get_instance().get(IconType::Play);
	ImTextureID resume_icon = EditorIcons::get_instance().get(IconType::Resume);
	ImTextureID pause_icon = EditorIcons::get_instance().get(IconType::Pause);
	ImTextureID stop_icon = EditorIcons::get_instance().get(IconType::Stop);
	ImTextureID step_icon = EditorIcons::get_instance().get(IconType::Step);

	int button_count = 2;

	if (m_controller.can_pause())
		button_count += 1;
	else if (m_controller.can_resume())
		button_count += 2;

	float button_width = button_size.x + style.FramePadding.x * 2.0f;
	float total_width = button_count * button_width + (button_count - 1) * style.ItemSpacing.x;

	float avail = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - total_width) * 0.5f);

	ImGui::BeginDisabled(!m_controller.can_play());
	if (ImGui::ImageButton("Play", play_icon, button_size))
		m_controller.play();
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(!m_controller.can_stop());
	if (ImGui::ImageButton("Stop", stop_icon, button_size))
		m_controller.stop();
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(!m_controller.can_pause() && !m_controller.can_resume());
	if (m_controller.can_pause()) {
		if (ImGui::ImageButton("Pause", pause_icon, button_size))
			m_controller.pause();
	} else if (m_controller.can_resume()) {
		if (ImGui::ImageButton("Resume", resume_icon, button_size))
			m_controller.resume();
		ImGui::SameLine();
		if (ImGui::ImageButton("Step", step_icon, button_size))
			m_controller.step();
	}
	ImGui::EndDisabled();

	ImGui::Separator();
}

}
