
#include "panels/RuntimeStatePanel.h"

#include <imgui.h>

#include "origo/assets/Texture.h"
#include "origo/assets/TextureSource.h"

namespace OrigoEditor {

static ImTextureID ToImTextureID(const Origo::Ref<Origo::Texture>& tex) {
	return (ImTextureID)(intptr_t)tex->GetRendererID();
}

static Origo::Ref<Origo::Texture> LoadSVGTexture(const std::string& path, int size = 18) {
	auto texture = Origo::MakeRef<Origo::Texture>(Origo::TextureType::UI);
	texture->SetSource(Origo::MakeScope<Origo::TextureSourceSVG>(path, size, size));
	texture->LoadCpuIfTextureNotExistent();
	return texture;
}

void RuntimeStatePanel::OnImGuiRender() {
	// --- lazy icon init ---
	static bool initialized = false;
	static Origo::Ref<Origo::Texture> playIcon;
	static Origo::Ref<Origo::Texture> pauseIcon;
	static Origo::Ref<Origo::Texture> stopIcon;

	if (!initialized) {
		playIcon = LoadSVGTexture("icons/Play.svg");
		pauseIcon = LoadSVGTexture("icons/Pause.svg");
		stopIcon = LoadSVGTexture("icons/Stop.svg");
		initialized = true;
	}

	// TODO: query real runtime state
	bool isEdit = true;
	bool isPlaying = false;
	bool isPaused = false;

	const ImVec2 buttonSize { 24.0f, 24.0f };
	const float spacing = ImGui::GetStyle().ItemSpacing.x;

	// total width: Play + Pause + Stop + spacing
	const float totalWidth = buttonSize.x * 3 + spacing * 2;

	const float availableWidth = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availableWidth - totalWidth) * 0.5f);

	// PLAY
	ImGui::BeginDisabled(!isEdit);
	if (ImGui::ImageButton(
	        "Play",
	        ToImTextureID(playIcon),
	        buttonSize)) {
		// TODO: clone editor scene
		// TODO: start runtime execution
		// TODO: switch active scene to runtime
	}
	ImGui::EndDisabled();

	ImGui::SameLine();

	// PAUSE / RESUME
	ImGui::BeginDisabled(isEdit);
	if (!isPaused) {
		if (ImGui::ImageButton(
		        "Pause",
		        ToImTextureID(pauseIcon),
		        buttonSize)) {
			// TODO: freeze runtime timestep
		}
	} else {
		if (ImGui::ImageButton(
		        "Resume",
		        ToImTextureID(playIcon),
		        buttonSize)) {
			// TODO: resume runtime timestep
		}
	}
	ImGui::EndDisabled();

	ImGui::SameLine();

	// STOP
	ImGui::BeginDisabled(isEdit);
	if (ImGui::ImageButton(
	        "Stop",
	        ToImTextureID(stopIcon),
	        buttonSize)) {
		// TODO: stop runtime execution
		// TODO: destroy runtime scene
		// TODO: restore editor scene
	}
	ImGui::EndDisabled();

	ImGui::Separator();
}

}
