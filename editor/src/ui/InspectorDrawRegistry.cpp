#include "ui/InspectorDrawRegistry.h"

#include "origo/scripting/ScriptComponentRegistry.h"

#include "imgui.h"
#include "origo/assets/Texture2D.h"
#include "ui/ComponentUI.h"

namespace OrigoEditor {

void InspectorDrawRegistry::DrawScriptComponent(Origo::ScriptComponentInstance& instance) {
	auto& componentDescription = Origo::ScriptComponentRegistry::Get(instance.ID);

	if (!DrawScriptComponentHeader(componentDescription.Name.c_str(), &instance))
		return;

	for (auto& field : componentDescription.Fields) {
		auto fieldValue = instance.FindValue(field.Name);

		// refactor!
		if (!fieldValue.has_value()) {
			continue;
		}

		DrawScriptField(field, *(*fieldValue));
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

#define SCRIPT_FIELD_CASE(TYPE, ACTUAL)                                              \
	case Origo::VariantType::TYPE: {                                             \
		ACTUAL v {};                                                         \
		if (!value.TryGetAs##TYPE(v)) {                                      \
			std::cout << "Trying to get as " #TYPE                       \
			          << " while its actually: "                         \
			          << magic_enum::enum_name(value.GetType()) << "\n"; \
			return;                                                      \
		}                                                                    \
		ComponentUI::Draw##TYPE##Control(field.Name, v);                     \
		value = Origo::Variant(v);                                           \
		break;                                                               \
	}

void InspectorDrawRegistry::DrawScriptField(const Origo::ScriptFieldDescriptor& field, Origo::Variant& value) {
	if (value.GetType() != field.Type) {
		value = field.DefaultValue;
	}

	switch (field.Type) {
		SCRIPT_FIELD_CASE(Bool, bool)
		SCRIPT_FIELD_CASE(Int, int)
		SCRIPT_FIELD_CASE(Float, float)
		SCRIPT_FIELD_CASE(String, std::string)
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
