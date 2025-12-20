#pragma once

#include "InspectorDrawable.h"
#include "imgui.h"
#include "origo/assets/Texture.h"
#include "ui/IInspectorDrawable.h"

#include <typeindex>
#include <unordered_map>
#include <string>

namespace OrigoEditor {

class InspectorDrawRegistry {
private:
	struct InspectorEntry {
		const char* Name;
		const char* IconPath;
		Origo::Ref<Origo::Texture> Icon;
		Origo::Scope<IInspectorDrawable> Drawer;
	};

public:
	template <typename T>
	static void Register(
	    const char* name,
	    const char* iconPath,
	    InspectorDrawable<T>::DrawFn fn) {
		InspectorEntry entry;
		entry.Name = name;
		entry.Drawer = Origo::MakeScope<InspectorDrawable<T>>(name, std::move(fn));
		entry.IconPath = iconPath;

		m_Drawers[typeid(T)] = std::move(entry);
	}

	static void Draw(void* drawablePtr, std::type_index type) {
		if (!m_UnkownIcon) {
			m_UnkownIcon = LoadIcon("./icons/Entity.svg");
		}
		auto it = m_Drawers.find(type);

		if (it == m_Drawers.end()) {
			ImGui::PushID(drawablePtr);

			const float iconSize = 18.0f;
			const float iconCellW = 22.0f;

			std::string headerLabel = "Unknown Component##" + std::to_string(reinterpret_cast<uintptr_t>(drawablePtr));

			if (ImGui::BeginTable("##inspector_row", 2,
			        ImGuiTableFlags_SizingFixedFit)) {
				ImGui::TableSetupColumn("##icon", ImGuiTableColumnFlags_WidthFixed, iconCellW);
				ImGui::TableSetupColumn("##header", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				{
					float yOff = (ImGui::GetFrameHeight() - iconSize) * 0.5f;
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOff);

					ImGui::Image(
					    (ImTextureID)(intptr_t)m_UnkownIcon->GetRendererID(),
					    ImVec2(iconSize, iconSize));
				}

				ImGui::TableSetColumnIndex(1);
				bool open = ImGui::CollapsingHeader(
				    headerLabel.c_str(),
				    ImGuiTreeNodeFlags_DefaultOpen);

				ImGui::EndTable();

				if (open) {
					ImGui::TextDisabled("(No inspector drawer registered)");
				}
			}

			ImGui::PopID();
			return;
		}

		InspectorEntry& entry = it->second;
		if (!entry.Icon) {
			entry.Icon = std::move(LoadIcon(entry.IconPath));
		}

		ImGui::PushID(drawablePtr);

		const float iconSize = 18.0f;
		const float iconCellW = 22.0f;

		std::string headerLabel = std::string(entry.Name) + "##" + std::to_string(reinterpret_cast<uintptr_t>(drawablePtr));

		if (ImGui::BeginTable("##inspector_row", 2,
		        ImGuiTableFlags_SizingFixedFit)) {
			ImGui::TableSetupColumn("##icon", ImGuiTableColumnFlags_WidthFixed, iconCellW);
			ImGui::TableSetupColumn("##header", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			{
				float yOff = (ImGui::GetFrameHeight() - iconSize) * 0.5f;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOff);

				ImGui::Image(
				    (ImTextureID)(intptr_t)entry.Icon->GetRendererID(),
				    ImVec2(iconSize, iconSize));
			}

			ImGui::TableSetColumnIndex(1);
			bool open = ImGui::CollapsingHeader(
			    headerLabel.c_str(),
			    ImGuiTreeNodeFlags_DefaultOpen);

			ImGui::EndTable();

			if (open) {
				entry.Drawer->Draw(drawablePtr);
			}
		}

		ImGui::PopID();
	}

private:
	static Origo::Ref<Origo::Texture> LoadIcon(const std::string& path) {
		auto tex = Origo::MakeRef<Origo::Texture>(Origo::TextureType::UI);
		tex->SetSource(Origo::MakeScope<Origo::TextureSourceSVG>(path, 14, 14));
		tex->LoadCpuIfTextureNotExistent();
		return tex;
	}

private:
	inline static std::unordered_map<std::type_index, InspectorEntry> m_Drawers;
	inline static Origo::Ref<Origo::Texture> m_UnkownIcon;
};

}
