#include "InspectorComponentRenderer.h"

#include "imgui.h"
#include "state/EditorContext.h"
#include "systems/EditorIcons.h"
#include "ui/ComponentUI.h"

#include "origo/assets/Texture2D.h"
#include "origo/scripting/ScriptComponentRegistry.h"

namespace OrigoEditor {
void InspectorComponentRenderer::Init(EditorContext* ctx) {
	s_Context = ctx;
}

void InspectorComponentRenderer::DrawNativeComponent(const Origo::RID& entity, void* componentPtr, std::type_index type) {
	auto* entry = InspectorDrawRegistry::Get(type);
	if (!entry) {
		DrawUnknown(componentPtr);
		return;
	}

	if (!entry->Icon && entry->IconPath)
		entry->Icon = LoadIcon(entry->IconPath);

	std::string label = std::string(entry->Name) + "##" + std::to_string(reinterpret_cast<uintptr_t>(componentPtr));

	if (DrawHeaderNative(entity, componentPtr, label.c_str(), entry->Icon, type))
		entry->Drawer->Draw(componentPtr);
}

void InspectorComponentRenderer::DrawScriptComponent(Origo::ScriptComponentInstance& instance) {
	auto& desc = Origo::ScriptComponentRegistry::Get(instance.ID);

	if (!s_ScriptIcon)
		s_ScriptIcon = LoadIcon("./icons/Script.svg");

	std::string label = desc.Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(&instance));

	if (!DrawHeaderScripted(&instance, label.c_str(), s_ScriptIcon))
		return;

	for (auto& fieldDesc : desc.Fields) {
		auto it = std::find_if(
		    instance.Fields.begin(),
		    instance.Fields.end(),
		    [&](auto& f) { return f.ID == fieldDesc.ID; });

		if (it == instance.Fields.end()) {
			instance.Fields.emplace_back(
			    fieldDesc.ID,
			    fieldDesc.Name,
			    fieldDesc.DefaultValue);

			it = std::prev(instance.Fields.end());
		}

		DrawScriptField(fieldDesc, it->Value);
	}
}

bool InspectorComponentRenderer::DrawHeaderNative(const Origo::RID& entity, void* id, const char* label, Origo::Ref<Origo::Texture2D> icon, std::type_index type) {
	ImGui::PushID(id);

	bool open = false;

	if (ImGui::BeginTable("##inspector_row", 2, ImGuiTableFlags_SizingFixedFit)) {
		ImGui::TableSetupColumn("##icon", ImGuiTableColumnFlags_WidthFixed, 24.0f);
		ImGui::TableSetupColumn("##header", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		float iconSize = 18.0f;
		float yOff = (ImGui::GetFrameHeight() - iconSize) * 0.5f;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOff);

		if (icon)
			ImGui::Image(
			    (ImTextureID)(intptr_t)icon->GetRendererID(),
			    ImVec2(iconSize, iconSize));

		ImGui::TableSetColumnIndex(1);

		open = ImGui::CollapsingHeader(
		    label,
		    ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

		ImGui::SetItemAllowOverlap();

		ImRect cellRect = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 1);

		float menuSize = 14.0f;
		float x = cellRect.Max.x
		    - menuSize
		    - ImGui::GetStyle().CellPadding.x
		    - ImGui::GetStyle().FramePadding.x;

		ImRect headerRect(
		    ImGui::GetItemRectMin(),
		    ImGui::GetItemRectMax());

		float y = headerRect.Min.y + (headerRect.GetHeight() - menuSize) * 0.5f;

		ImGui::SetCursorScreenPos(ImVec2(x, y));
		ImGui::SetNextItemAllowOverlap();

		ImTextureID menuIcon = EditorIcons::Get(IconType::Menu);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(
		    ImGuiCol_ButtonHovered,
		    ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered));
		ImGui::PushStyleColor(
		    ImGuiCol_ButtonActive,
		    ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));

		if (ImGui::ImageButton(
		        "##component_menu_btn",
		        menuIcon,
		        ImVec2(menuSize, menuSize))) {

			ImGui::OpenPopup("##component_menu");
		}

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);

		if (ImGui::BeginPopup("##component_menu")) {
			if (ImGui::MenuItem("Remove Component")) {
				s_Context->ActiveScene->ScheduleRemoveNativeComponent(entity, type);
			}
			ImGui::EndPopup();
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
	return open;
}

bool InspectorComponentRenderer::DrawHeaderScripted(void* id, const char* label, Origo::Ref<Origo::Texture2D> icon) {
	ImGui::PushID(id);

	bool open = ImGui::CollapsingHeader(
	    label,
	    ImGuiTreeNodeFlags_DefaultOpen);

	ImGui::PopID();
	return open;
}

void InspectorComponentRenderer::DrawUnknown(void* componentPtr) {
	if (!s_UnknownIcon)
		s_UnknownIcon = LoadIcon("./icons/Entity.svg");

	std::string label = "Unknown Component##" + std::to_string(reinterpret_cast<uintptr_t>(componentPtr));

	ImGui::TextDisabled("(No inspector drawer registered)");
}

void InspectorComponentRenderer::DrawScriptField(const Origo::ScriptFieldDescriptor& field, Origo::Variant& value) {
	if (value.GetType() != field.Type)
		value = field.DefaultValue;

	switch (field.Type) {
	case Origo::VariantType::Bool: {
		bool v;
		value.TryGetAsBool(v);
		ComponentUI::DrawBoolControl(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::Int: {
		int v;
		value.TryGetAsInt(v);
		ComponentUI::DrawIntControl(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::Float: {
		float v;
		value.TryGetAsFloat(v);
		ComponentUI::DrawFloatControl(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::String: {
		std::string v;
		value.TryGetAsString(v);
		ComponentUI::DrawStringControl(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	}
}

Origo::Ref<Origo::Texture2D> InspectorComponentRenderer::LoadIcon(const std::string& path) {
	auto tex = Origo::MakeRef<Origo::Texture2D>(
	    Origo::TextureType::UI);

	tex->SetSource(
	    Origo::MakeScope<Origo::TextureSourceSVG>(
	        path, 14, 14));

	tex->Load();
	return tex;
}
}
