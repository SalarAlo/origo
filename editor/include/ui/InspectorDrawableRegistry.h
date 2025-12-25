#pragma once

#include "InspectorDrawable.h"
#include "ui/IInspectorDrawable.h"

#include "imgui.h"
#include "origo/assets/Texture2D.h"

#include <typeindex>
#include <unordered_map>
#include <string>

namespace OrigoEditor {

class InspectorDrawRegistry {
private:
	struct InspectorEntry {
		const char* Name = nullptr;
		const char* IconPath = nullptr;
		Origo::Ref<Origo::Texture2D> Icon;
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
		entry.IconPath = iconPath;
		entry.Drawer = Origo::MakeScope<InspectorDrawable<T>>(name, std::move(fn));

		m_Drawers[typeid(T)] = std::move(entry);
	}

	static void Draw(void* drawablePtr, std::type_index type) {
		EnsureUnknownIcon();

		auto it = m_Drawers.find(type);
		if (it == m_Drawers.end()) {
			DrawUnknown(drawablePtr);
			return;
		}

		DrawKnown(drawablePtr, it->second);
	}

	static const auto& GetEntries() {
		return m_Drawers;
	}

private:
	static bool DrawInspectorHeaderRow(
	    void* idPtr,
	    ImTextureID icon,
	    const char* label,
	    float iconSize = 18.0f,
	    float iconCellW = 22.0f) {
		ImGui::PushID(idPtr);
		bool open = false;

		if (ImGui::BeginTable("##inspector_row", 2, ImGuiTableFlags_SizingFixedFit)) {
			ImGui::TableSetupColumn("##icon", ImGuiTableColumnFlags_WidthFixed, iconCellW);
			ImGui::TableSetupColumn("##header", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			{
				float yOff = (ImGui::GetFrameHeight() - iconSize) * 0.5f;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOff);
				ImGui::Image(icon, ImVec2(iconSize, iconSize));
			}

			ImGui::TableSetColumnIndex(1);
			open = ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen);

			ImGui::EndTable();
		}

		ImGui::PopID();
		return open;
	}

	static void DrawUnknown(void* drawablePtr) {
		std::string label = "Unknown Component##" + std::to_string(reinterpret_cast<uintptr_t>(drawablePtr));

		bool open = DrawInspectorHeaderRow(
		    drawablePtr,
		    (ImTextureID)(intptr_t)m_UnknownIcon->GetRendererID(),
		    label.c_str());

		if (open) {
			ImGui::TextDisabled("(No inspector drawer registered)");
		}
	}

	static void DrawKnown(void* drawablePtr, InspectorEntry& entry) {
		EnsureEntryIcon(entry);

		std::string label = std::string(entry.Name) + "##" + std::to_string(reinterpret_cast<uintptr_t>(drawablePtr));

		bool open = DrawInspectorHeaderRow(
		    drawablePtr,
		    (ImTextureID)(intptr_t)entry.Icon->GetRendererID(),
		    label.c_str());

		if (open) {
			entry.Drawer->Draw(drawablePtr);
		}
	}

	static void EnsureUnknownIcon() {
		if (!m_UnknownIcon) {
			m_UnknownIcon = LoadIcon("./icons/Entity.svg");
		}
	}

	static void EnsureEntryIcon(InspectorEntry& entry) {
		if (!entry.Icon && entry.IconPath) {
			entry.Icon = LoadIcon(entry.IconPath);
		}
	}

	static Origo::Ref<Origo::Texture2D> LoadIcon(const std::string& path) {
		auto tex = Origo::MakeRef<Origo::Texture2D>(Origo::TextureType::UI);
		tex->SetSource(Origo::MakeScope<Origo::TextureSourceSVG>(path, 14, 14));
		tex->Load();
		return tex;
	}

private:
	inline static std::unordered_map<std::type_index, InspectorEntry> m_Drawers;
	inline static Origo::Ref<Origo::Texture2D> m_UnknownIcon;
};

}
