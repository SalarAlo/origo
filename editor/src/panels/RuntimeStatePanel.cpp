#include "panels/RuntimeStatePanel.h"

#include "origo/assets/Texture2D.h"
#include "origo/assets/TextureSource.h"
#include "state/EditorRuntimeState.h"

namespace OrigoEditor {

static ImTextureID ToImTextureID(const Origo::Ref<Origo::Texture2D>& tex) {
	return (ImTextureID)(intptr_t)tex->GetRendererID();
}

static Origo::Ref<Origo::Texture2D> LoadSVGTexture(const std::string& path, int size = 18) {
	auto texture = Origo::MakeRef<Origo::Texture2D>(Origo::TextureType::UI);
	texture->SetSource(Origo::MakeScope<Origo::TextureSourceSVG>(path, size, size));
	texture->Load();
	return texture;
}

void RuntimeStatePanel::OnImGuiRender() {
	static bool initialized = false;
	static Origo::Ref<Origo::Texture2D> playIcon;
	static Origo::Ref<Origo::Texture2D> pauseIcon;
	static Origo::Ref<Origo::Texture2D> stopIcon;

	if (!initialized) {
		playIcon = LoadSVGTexture("icons/Play.svg");
		pauseIcon = LoadSVGTexture("icons/Pause.svg");
		stopIcon = LoadSVGTexture("icons/Stop.svg");
		initialized = true;
	}

	bool isEdit = m_Context.RuntimeState == EditorRuntimeState::Editing;
	bool isPlaying = m_Context.RuntimeState == EditorRuntimeState::Running;

	const ImVec2 buttonSize { 24.0f, 24.0f };
	const float spacing = ImGui::GetStyle().ItemSpacing.x;

	const float totalWidth = buttonSize.x * 3 + spacing * 2;

	const float availableWidth = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availableWidth - totalWidth) * 0.5f);

	ImGui::BeginDisabled(!isEdit);
	if (ImGui::ImageButton(
	        "Play",
	        ToImTextureID(playIcon),
	        buttonSize)) {
		m_Controller.Play();
	}
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(isEdit);
	if (ImGui::ImageButton(
	        "Stop",
	        ToImTextureID(stopIcon),
	        buttonSize)) {
		m_Controller.Stop();
	}
	ImGui::EndDisabled();

	ImGui::Separator();
}

}
