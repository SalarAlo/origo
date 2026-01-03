#include "panels/RuntimeStatePanel.h"

#include "state/EditorContext.h"
#include "systems/EditorIcons.h"

namespace OrigoEditor {

RuntimeStatePanel::RuntimeStatePanel(EditorContext& context)
    : m_Context(context)
    , m_Controller(context) {
}

void RuntimeStatePanel::OnImGuiRender() {
	const ImVec2 buttonSize { 24.0f, 24.0f };
	const auto& style = ImGui::GetStyle();

	ImTextureID playIcon = EditorIcons::Get(IconType::Play);
	ImTextureID resumeIcon = EditorIcons::Get(IconType::Resume);
	ImTextureID pauseIcon = EditorIcons::Get(IconType::Pause);
	ImTextureID stopIcon = EditorIcons::Get(IconType::Stop);
	ImTextureID stepIcon = EditorIcons::Get(IconType::Step);

	int buttonCount = 2;

	if (m_Controller.CanPause())
		buttonCount += 1;
	else if (m_Controller.CanResume())
		buttonCount += 2;
	else
		buttonCount += 1;

	float buttonWidth = buttonSize.x + style.FramePadding.x * 2.0f;
	float totalWidth = buttonCount * buttonWidth + (buttonCount - 1) * style.ItemSpacing.x;

	float avail = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - totalWidth) * 0.5f);

	ImGui::BeginDisabled(!m_Controller.CanPlay());
	if (ImGui::ImageButton("Play", playIcon, buttonSize))
		m_Controller.Play();
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(!m_Controller.CanStop());
	if (ImGui::ImageButton("Stop", stopIcon, buttonSize))
		m_Controller.Stop();
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(!m_Controller.CanPause() && !m_Controller.CanResume());
	if (m_Controller.CanPause()) {
		if (ImGui::ImageButton("Pause", pauseIcon, buttonSize))
			m_Controller.Pause();
	} else if (m_Controller.CanResume()) {
		if (ImGui::ImageButton("Resume", resumeIcon, buttonSize))
			m_Controller.Resume();
		ImGui::SameLine();
		if (ImGui::ImageButton("Step", stepIcon, buttonSize))
			m_Controller.Step();
	}
	ImGui::EndDisabled();

	ImGui::Separator();
}

}
