#include "ui/InspectorDrawRegistry.h"

#include "origo/scripting/ScriptComponentRegistry.h"
#include "ui/ComponentUI.h"

#include "imgui.h"
#include "origo/assets/Texture2D.h"

namespace OrigoEditor {

void InspectorDrawRegistry::DrawScriptComponent(
    Origo::ScriptComponentInstance& instance) {
	const auto& desc = Origo::ScriptComponentRegistry::Get(instance.ID);

	if (!DrawScriptComponentHeader(desc.Name.c_str(), &instance))
		return;

	for (size_t i = 0; i < desc.Fields.size(); ++i) {
		DrawScriptField(desc.Fields[i], instance.Values[i]);
	}
}

void InspectorDrawRegistry::DrawNativeComponent(void* componentPtr, std::type_index type) {
	EnsureUnknownIcon();

	auto it = m_Drawers.find(type);
	if (it == m_Drawers.end()) {
		DrawUnknown(componentPtr);
		return;
	}

	DrawKnown(componentPtr, it->second);
}

bool InspectorDrawRegistry::DrawScriptComponentHeader(
    const char* name, void* idPtr) {
	EnsureScriptIcon();

	std::string label = std::string(name) + "##" + std::to_string(reinterpret_cast<uintptr_t>(idPtr));

	return DrawInspectorHeaderRow(
	    idPtr,
	    (ImTextureID)(intptr_t)m_ScriptIcon->GetRendererID(),
	    label.c_str());
}

bool InspectorDrawRegistry::DrawInspectorHeaderRow(
    void* idPtr,
    ImTextureID icon,
    const char* label,
    float iconSize,
    float iconCellW) {
	ImGui::PushID(idPtr);
	bool open = false;

	if (ImGui::BeginTable("##inspector_row", 2, ImGuiTableFlags_SizingFixedFit)) {
		ImGui::TableSetupColumn("##icon", ImGuiTableColumnFlags_WidthFixed, iconCellW);
		ImGui::TableSetupColumn("##header", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		float yOff = (ImGui::GetFrameHeight() - iconSize) * 0.5f;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOff);
		ImGui::Image(icon, ImVec2(iconSize, iconSize));

		ImGui::TableSetColumnIndex(1);
		open = ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::EndTable();
	}

	ImGui::PopID();
	return open;
}

void InspectorDrawRegistry::DrawScriptField(
    const Origo::ScriptFieldDescriptor& field,
    Origo::Variant& value) {
	switch (field.Type) {
	case Origo::VariantType::Bool: {
		bool v = value.GetAsBool();
		ComponentUI::DrawBoolControl(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::Int: {
		int v = value.GetAsInt();
		ComponentUI::DrawIntControl(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::Float: {
		float v = value.GetAsFloat();
		ComponentUI::DrawFloatControl(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::String: {
		std::string v = value.GetAsString();
		ComponentUI::DrawStringControl(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	}
}

void InspectorDrawRegistry::DrawUnknown(void* drawablePtr) {
	std::string label = "Unknown Component##" + std::to_string(reinterpret_cast<uintptr_t>(drawablePtr));

	bool open = DrawInspectorHeaderRow(
	    drawablePtr,
	    (ImTextureID)(intptr_t)m_UnknownIcon->GetRendererID(),
	    label.c_str());

	if (open) {
		ImGui::TextDisabled("(No inspector drawer registered)");
	}
}

void InspectorDrawRegistry::DrawKnown(
    void* drawablePtr,
    InspectorEntry& entry) {
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

void InspectorDrawRegistry::EnsureUnknownIcon() {
	if (!m_UnknownIcon) {
		m_UnknownIcon = LoadIcon("./icons/Entity.svg");
	}
}

void InspectorDrawRegistry::EnsureScriptIcon() {
	if (!m_ScriptIcon) {
		m_ScriptIcon = LoadIcon("./icons/Script.svg");
	}
}

void InspectorDrawRegistry::EnsureEntryIcon(
    InspectorEntry& entry) {
	if (!entry.Icon && entry.IconPath) {
		entry.Icon = LoadIcon(entry.IconPath);
	}
}

Origo::Ref<Origo::Texture2D>
InspectorDrawRegistry::LoadIcon(const std::string& path) {
	auto tex = Origo::MakeRef<Origo::Texture2D>(Origo::TextureType::UI);
	tex->SetSource(
	    Origo::MakeScope<Origo::TextureSourceSVG>(path, 14, 14));
	tex->Load();
	return tex;
}

}
