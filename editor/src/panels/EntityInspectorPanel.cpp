#include <typeindex>

#include "panels/EntityInspectorPanel.h"

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"

#include "origo/components/Name.h"
#include "origo/components/NativeComponentRegistry.h"

#include "origo/scripting/ScriptComponentRegistry.h"

#include "ui/InspectorComponentRenderer.h"
#include "ui/InspectorDrawRegistry.h"

using namespace Origo;

namespace OrigoEditor {

void EntityInspectorPanel::on_im_gui_render() {
	auto selected_entity_optional = m_context.get_selected_entity();
	auto active_scene = m_context.ActiveScene;

	if (!selected_entity_optional.has_value()) {
		ImGui::TextDisabled("No entity selected.");
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	draw_entity_name();

	const Origo::RID selected_entity = selected_entity_optional.value();

	ImGui::SetWindowFontScale(0.9f);

	draw_native_components(active_scene, selected_entity);
	draw_script_components(active_scene, selected_entity);

	ImGui::SetWindowFontScale(1.0f);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	draw_add_component(active_scene, selected_entity);
	draw_script_drop_target(active_scene, selected_entity);
}

void EntityInspectorPanel::draw_entity_name() {
	auto selected_entity = m_context.get_selected_entity();
	auto scene = m_context.ActiveScene;

	const Origo::RID entity = selected_entity.value();
	auto* name_comp = scene->get_native_component<Origo::NameComponent>(entity);
	const std::string& name = name_comp->Name;

	static Origo::RID editing_entity {};
	static bool is_editing_name = false;
	static char name_buffer[256] {};

	ImGui::SetWindowFontScale(1.1f);

	if (!is_editing_name || editing_entity != entity) {
		is_editing_name = false;
	}

	if (!is_editing_name) {
		ImGui::SeparatorText(name.c_str());

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			editing_entity = entity;
			is_editing_name = true;
			std::strncpy(name_buffer, name.c_str(), sizeof(name_buffer));
			name_buffer[sizeof(name_buffer) - 1] = '\0';
			ImGui::SetKeyboardFocusHere();
		}
	} else {
		ImGui::SetNextItemWidth(-1.0f);

		const bool enter_pressed = ImGui::InputText("##EntityName",
		    name_buffer,
		    sizeof(name_buffer),
		    ImGuiInputTextFlags_EnterReturnsTrue);

		const bool deactivated = ImGui::IsItemDeactivatedAfterEdit();

		if (enter_pressed || deactivated) {
			name_comp->Name = name_buffer;
			is_editing_name = false;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
			std::strncpy(name_buffer, name.c_str(), sizeof(name_buffer));
			is_editing_name = false;
		}
	}
}

void EntityInspectorPanel::draw_native_components(Origo::Scene* activeScene, Origo::RID selectedEntity) {
	for (const auto& [type, entry] : InspectorDrawRegistry::get_instance().get_entries()) {
		if (!activeScene->has_native_component_by_type(selectedEntity, type))
			continue;

		void* component_ptr = activeScene->get_native_component_by_type(selectedEntity, type);
		InspectorComponentRenderer::get_instance().draw_native_component(selectedEntity, component_ptr, type);
	}
}

void EntityInspectorPanel::draw_script_components(Origo::Scene* activeScene, Origo::RID selectedEntity) {
	for (const auto& [id, descr] : Origo::ScriptComponentRegistry::get_all()) {
		if (!activeScene->has_script_component(selectedEntity, id))
			continue;

		auto& instance = *activeScene->get_script_component(selectedEntity, id);
		InspectorComponentRenderer::get_instance().draw_script_component(instance);
	}
}

void EntityInspectorPanel::draw_add_component(Origo::Scene* activeScene, Origo::RID selectedEntity) {
	const float button_width = 180.0f;
	const float content_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;

	ImGui::SetCursorPosX(
	    ImGui::GetWindowContentRegionMin().x + (content_width - button_width) * 0.5f);

	static ImVec2 add_button_pos {};
	static ImVec2 add_button_size {};

	if (ImGui::Button("+ Add Component", ImVec2(button_width, 0))) {
		add_button_pos = ImGui::GetItemRectMin();
		add_button_size = ImGui::GetItemRectSize();
		ImGui::OpenPopup("AddComponentPopup");
	}

	ImGui::SetNextWindowPos(
	    ImVec2(add_button_pos.x, add_button_pos.y + add_button_size.y),
	    ImGuiCond_Appearing);

	ImGui::SetNextWindowSize(ImVec2(300.0f, 0.0f),
	    ImGuiCond_Appearing);

	if (ImGui::BeginPopup("AddComponentPopup",
	        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {

		ImGui::TextDisabled("Available Components");
		ImGui::Separator();

		for (const auto& [type, entry] : InspectorDrawRegistry::get_instance().get_entries()) {
			if (activeScene->has_native_component_by_type(selectedEntity, type))
				continue;

			const bool is_registered = Origo::NativeComponentRegistry::get_instance().get_component_info(type) != nullptr;

			if (!is_registered) {
				ImGui::PushStyleColor(
				    ImGuiCol_Text,
				    ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
				ImGui::BeginDisabled();
				ImGui::MenuItem(entry.Name);
				ImGui::EndDisabled();
				ImGui::PopStyleColor();
				continue;
			}

			if (ImGui::MenuItem(entry.Name)) {
				activeScene->add_native_component(selectedEntity, type);
				ImGui::CloseCurrentPopup();
				break;
			}
		}

		ImGui::Separator();
		ImGui::TextDisabled("Script Components");

		for (const auto& [id, desc] : Origo::ScriptComponentRegistry::get_all()) {
			if (activeScene->has_script_component(selectedEntity, id))
				continue;

			if (ImGui::MenuItem(desc.Name.c_str())) {
				activeScene->add_script_component(selectedEntity, id);
				ImGui::CloseCurrentPopup();
				break;
			}
		}

		ImGui::EndPopup();
	}
}

void EntityInspectorPanel::draw_script_drop_target(
    Origo::Scene* activeScene,
    Origo::RID selectedEntity) {
	ImGui::Spacing();

	ImVec2 drop_size(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	ImGui::Dummy(drop_size);

	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();

	ImDrawList* dl = ImGui::GetWindowDrawList();

	const bool hovering_payload = ImGui::GetDragDropPayload() != nullptr && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

	if (hovering_payload) {
		dl->AddRect(
		    min, max,
		    IM_COL32(120, 160, 255, 180),
		    4.0f, 0, 2.0f);

		dl->AddText(
		    ImVec2(min.x + 8, min.y + 8),
		    IM_COL32(180, 200, 255, 220),
		    "Drop script to add");
	}

	if (!ImGui::BeginDragDropTarget())
		return;

	const ImGuiPayload* payload {};
	if (payload = ImGui::AcceptDragDropPayload("ORIGO_ASSET_UUID"); !payload) {
		ImGui::EndDragDropTarget();
		return;
	}

	const char* uuid_str = static_cast<const char*>(payload->Data);
	Origo::UUID uuid = Origo::UUID::from_string(uuid_str);

	auto metadata = Origo::AssetDatabase::get_instance().get_metadata(uuid);
	if (metadata.Type == Origo::AssetType::Script) {

		auto script_id = Origo::ScriptComponentRegistry::get(uuid);
		if (!activeScene->has_script_component(selectedEntity, script_id.ID)) {
			activeScene->add_script_component(selectedEntity, script_id.ID);
		}
	}
}

}
