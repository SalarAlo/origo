#include "InspectorComponentRenderer.h"

#include "imgui.h"

#include "origo/assets/Texture2D.h"

#include "origo/scripting/ScriptComponentRegistry.h"

#include "state/EditorContext.h"

#include "systems/EditorIcons.h"

#include "ui/ComponentUI.h"

namespace OrigoEditor {
void InspectorComponentRenderer::init(EditorContext* ctx) {
	s_context = ctx;
}

void InspectorComponentRenderer::draw_native_component(const Origo::RID& entity, void* componentPtr, std::type_index type) {
	auto* entry = InspectorDrawRegistry::get(type);

	if (!entry) {
		draw_unknown(componentPtr);
		return;
	}

	if (!entry->Icon && entry->IconPath)
		entry->Icon = load_icon(entry->IconPath);

	std::string label = std::string(entry->Name) + "##" + std::to_string(reinterpret_cast<uintptr_t>(componentPtr));

	if (draw_header_native(entity, componentPtr, label.c_str(), entry->Icon, type))
		entry->Drawer->draw(componentPtr);
}

void InspectorComponentRenderer::draw_script_component(Origo::ScriptComponentInstance& instance) {
	auto& desc = Origo::ScriptComponentRegistry::get(instance.ID);

	if (!s_script_icon)
		s_script_icon = load_icon("./icons/Script.svg");

	std::string label = desc.Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(&instance));

	if (!draw_header_scripted(&instance, label.c_str(), s_script_icon))
		return;

	for (auto& field_desc : desc.Fields) {
		auto it = std::find_if(
		    instance.Fields.begin(),
		    instance.Fields.end(),
		    [&](auto& f) { return f.ID == field_desc.ID; });

		if (it == instance.Fields.end()) {
			instance.Fields.emplace_back(
			    field_desc.ID,
			    field_desc.Name,
			    field_desc.DefaultValue);

			it = std::prev(instance.Fields.end());
		}

		draw_script_field(field_desc, it->Value);
	}
}

bool InspectorComponentRenderer::draw_header_native(const Origo::RID& entity, void* id, const char* label, Origo::Ref<Origo::Texture2D> icon, std::type_index type) {
	ImGui::PushID(id);

	bool open = false;

	if (ImGui::BeginTable("##inspector_row", 2, ImGuiTableFlags_SizingFixedFit)) {
		ImGui::TableSetupColumn("##icon", ImGuiTableColumnFlags_WidthFixed, 24.0f);
		ImGui::TableSetupColumn("##header", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		float icon_size = 18.0f;
		float y_off = (ImGui::GetFrameHeight() - icon_size) * 0.5f;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y_off);

		if (icon)
			ImGui::Image(
			    (ImTextureID)(intptr_t)icon->get_renderer_id(),
			    ImVec2(icon_size, icon_size));

		ImGui::TableSetColumnIndex(1);

		open = ImGui::CollapsingHeader(
		    label,
		    ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

		ImGui::SetItemAllowOverlap();

		ImRect cell_rect = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 1);

		float menu_size = 14.0f;
		float x = cell_rect.Max.x
		    - menu_size
		    - ImGui::GetStyle().CellPadding.x
		    - ImGui::GetStyle().FramePadding.x;

		ImRect header_rect(
		    ImGui::GetItemRectMin(),
		    ImGui::GetItemRectMax());

		float y = header_rect.Min.y + (header_rect.GetHeight() - menu_size) * 0.5f;

		ImGui::SetCursorScreenPos(ImVec2(x, y));
		ImGui::SetNextItemAllowOverlap();

		ImTextureID menu_icon = EditorIcons::get_instance().get(IconType::Menu);

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
		        menu_icon,
		        ImVec2(menu_size, menu_size))) {

			ImGui::OpenPopup("##component_menu");
		}

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);

		if (ImGui::BeginPopup("##component_menu")) {
			if (ImGui::MenuItem("Remove Component")) {
				s_context->ActiveScene->schedule_remove_native_component(entity, type);
			}
			ImGui::EndPopup();
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
	return open;
}

bool InspectorComponentRenderer::draw_header_scripted(void* id, const char* label, Origo::Ref<Origo::Texture2D> icon) {
	ImGui::PushID(id);

	bool open = ImGui::CollapsingHeader(
	    label,
	    ImGuiTreeNodeFlags_DefaultOpen);

	ImGui::PopID();
	return open;
}

void InspectorComponentRenderer::draw_unknown(void* componentPtr) {
	if (!s_unknown_icon)
		s_unknown_icon = load_icon("./icons/Entity.svg");

	std::string label = "Unknown Component##" + std::to_string(reinterpret_cast<uintptr_t>(componentPtr));

	ImGui::TextDisabled("(No inspector drawer registered)");
}

void InspectorComponentRenderer::draw_script_field(const Origo::ScriptFieldDescriptor& field, Origo::Variant& value) {
	if (value.get_type() != field.Type)
		value = field.DefaultValue;

	switch (field.Type) {
	case Origo::VariantType::Bool: {
		bool v;
		value.TryGetAsBool(v);
		ComponentUI::draw_bool_control(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::Int: {
		int v;
		value.TryGetAsInt(v);
		ComponentUI::draw_int_control(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::Float: {
		float v;
		value.TryGetAsFloat(v);
		ComponentUI::draw_float_control(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::String: {
		std::string v;
		value.TryGetAsString(v);
		ComponentUI::draw_string_control(field.Name, v);
		value = Origo::Variant(v);
		break;
	}
	}
}

Origo::Ref<Origo::Texture2D> InspectorComponentRenderer::load_icon(const std::string& path) {
	auto tex = Origo::MakeRef<Origo::Texture2D>(
	    Origo::TextureType::UI);

	tex->set_source(
	    Origo::MakeScope<Origo::TextureSourceSVG>(
	        path, 14, 14));

	tex->load();
	return tex;
}
}
