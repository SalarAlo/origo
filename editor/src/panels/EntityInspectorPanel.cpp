#include <cctype>

#include <algorithm>
#include <string_view>
#include <typeindex>
#include <vector>

#include "panels/EntityInspectorPanel.h"

#include "imgui.h"

#include "misc/cpp/imgui_stdlib.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"

#include "origo/components/NameComponent.h"
#include "origo/components/NativeComponentRegistry.h"

#include "origo/scripting/ScriptComponentRegistry.h"

#include "ui/InspectorComponentRenderer.h"
#include "ui/InspectorDrawRegistry.h"

using namespace Origo;

namespace OrigoEditor {

namespace {
	bool text_matches_query(std::string_view text, std::string_view query) {
		if (query.empty())
			return true;

		return std::search(
		           text.begin(),
		           text.end(),
		           query.begin(),
		           query.end(),
		           [](char lhs, char rhs) {
			           return std::tolower((unsigned char)lhs) == std::tolower((unsigned char)rhs);
		           })
		    != text.end();
	}

	struct NativePickerEntry {
		const char* Name = nullptr;
		std::type_index Type = typeid(void);
	};

	struct ScriptPickerEntry {
		const char* Name = nullptr;
		Origo::ScriptComponentID ID;
	};

	bool draw_picker_row(const char* label, const char* category_label, bool enabled) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 6.0f));

		if (!enabled)
			ImGui::BeginDisabled();

		const bool selected = ImGui::Selectable(label, false, ImGuiSelectableFlags_SpanAvailWidth);
		const ImVec2 min = ImGui::GetItemRectMin();
		const ImVec2 max = ImGui::GetItemRectMax();

		if (!enabled)
			ImGui::EndDisabled();

		const ImU32 category_color = ImGui::GetColorU32(enabled ? ImGuiCol_TextDisabled : ImGuiCol_Text);
		const ImVec2 text_size = ImGui::CalcTextSize(category_label);
		ImGui::GetWindowDrawList()->AddText(
		    ImVec2(max.x - text_size.x - 10.0f, min.y + 6.0f),
		    category_color,
		    category_label);

		ImGui::PopStyleVar();
		return enabled && selected;
	}
}

void EntityInspectorPanel::on_im_gui_render() {
	auto selected_entity_optional = m_context.get_selected_entity();
	auto active_scene = m_context.ActiveScene;

	if (!active_scene) {
		ImGui::TextDisabled("No active scene.");
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	if (!selected_entity_optional.has_value()) {
		ImGui::TextDisabled("No entity selected.");
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	const Origo::RID selected_entity = selected_entity_optional.value();
	if (!active_scene->has_entity(selected_entity)) {
		m_context.unselect_entity();
		ImGui::TextDisabled("No entity selected.");
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	draw_entity_name(active_scene, selected_entity);

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

void EntityInspectorPanel::draw_entity_name(Origo::Scene* scene, Origo::RID entity) {
	auto* name_comp = scene->get_native_component<Origo::NameComponent>(entity);

	static Origo::RID editing_entity {};
	static bool is_editing_name = false;
	static char name_buffer[256] {};

	ImGui::SetWindowFontScale(1.1f);

	if (!is_editing_name || editing_entity != entity) {
		is_editing_name = false;
	}

	if (!name_comp) {
		ImGui::SeparatorText("Unnamed Entity");
		return;
	}

	const std::string& name = name_comp->Name;

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

		if (ImGui::IsItemActive())
			m_context.mark_text_input_active();

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
		if (!component_ptr)
			continue;

		InspectorComponentRenderer::get_instance().draw_native_component(selectedEntity, component_ptr, type);
	}
}

void EntityInspectorPanel::draw_script_components(Origo::Scene* activeScene, Origo::RID selectedEntity) {
	for (const auto& [id, descr] : Origo::ScriptComponentRegistry::get_all()) {
		if (!activeScene->has_script_component(selectedEntity, id))
			continue;

		auto* component = activeScene->get_script_component(selectedEntity, id);
		if (!component)
			continue;

		auto& instance = *component;
		InspectorComponentRenderer::get_instance().draw_script_component(instance);
	}
}

void EntityInspectorPanel::draw_add_component(Origo::Scene* activeScene, Origo::RID selectedEntity) {
	const float button_width = std::min(220.0f, ImGui::GetContentRegionAvail().x);
	const float content_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;

	ImGui::SetCursorPosX(
	    ImGui::GetWindowContentRegionMin().x + (content_width - button_width) * 0.5f);

	static ImVec2 add_button_pos {};
	static ImVec2 add_button_size {};

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 8.0f));
	if (ImGui::Button("Add Component", ImVec2(button_width, 0))) {
		add_button_pos = ImGui::GetItemRectMin();
		add_button_size = ImGui::GetItemRectSize();

		m_add_component_query.clear();
		m_focus_add_component_search = true;

		ImGui::OpenPopup("AddComponentPopup");
	}
	ImGui::PopStyleVar();

	ImGui::SetNextWindowPos(
	    ImVec2(add_button_pos.x, add_button_pos.y + add_button_size.y),
	    ImGuiCond_Appearing);

	ImGui::SetNextWindowSize(ImVec2(380.0f, 420.0f),
	    ImGuiCond_Appearing);

	if (ImGui::BeginPopup("AddComponentPopup",
	        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		std::vector<NativePickerEntry> available_native;
		std::vector<NativePickerEntry> unavailable_native;
		std::vector<ScriptPickerEntry> available_script;

		for (const auto& [type, entry] : InspectorDrawRegistry::get_instance().get_entries()) {
			if (activeScene->has_native_component_by_type(selectedEntity, type))
				continue;

			const bool is_registered = Origo::NativeComponentRegistry::get_instance().get_component_info(type) != nullptr;
			(is_registered ? available_native : unavailable_native).push_back(NativePickerEntry { .Name = entry.Name, .Type = type });
		}

		for (const auto& [id, desc] : Origo::ScriptComponentRegistry::get_all()) {
			if (activeScene->has_script_component(selectedEntity, id))
				continue;

			available_script.push_back(ScriptPickerEntry { .Name = desc.Name.c_str(), .ID = id });
		}

		auto sort_by_name = [](const auto& lhs, const auto& rhs) {
			return std::string_view(lhs.Name) < std::string_view(rhs.Name);
		};

		std::sort(available_native.begin(), available_native.end(), sort_by_name);
		std::sort(unavailable_native.begin(), unavailable_native.end(), sort_by_name);
		std::sort(available_script.begin(), available_script.end(), sort_by_name);

		ImGui::TextUnformatted("Add Component");
		ImGui::SameLine();
		ImGui::TextDisabled("%zu available", available_native.size() + available_script.size());
		ImGui::Separator();

		ImGui::SetNextItemWidth(-1.0f);
		const bool search_changed = ImGui::InputTextWithHint(
		    "##AddComponentSearch", "Search components...", &m_add_component_query);
		if (m_focus_add_component_search) {
			ImGui::SetKeyboardFocusHere(-1);
			m_focus_add_component_search = false;
		}

		if (ImGui::IsItemActive())
			m_context.mark_text_input_active();

		ImGui::Spacing();

		std::size_t visible_available_native = 0;
		std::size_t visible_unavailable_native = 0;
		std::size_t visible_script = 0;

		for (const auto& item : available_native)
			visible_available_native += text_matches_query(item.Name, m_add_component_query) ? 1 : 0;
		for (const auto& item : unavailable_native)
			visible_unavailable_native += text_matches_query(item.Name, m_add_component_query) ? 1 : 0;
		for (const auto& item : available_script)
			visible_script += text_matches_query(item.Name, m_add_component_query) ? 1 : 0;

		const std::size_t visible_addable_total = visible_available_native + visible_script;
		const std::size_t visible_total = visible_available_native + visible_unavailable_native + visible_script;
		bool component_added = false;

		if (search_changed && !m_add_component_query.empty() && visible_addable_total == 1) {
			for (const auto& item : available_native) {
				if (!text_matches_query(item.Name, m_add_component_query))
					continue;

				activeScene->add_native_component(selectedEntity, item.Type);
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return;
			}

			for (const auto& item : available_script) {
				if (!text_matches_query(item.Name, m_add_component_query))
					continue;

				activeScene->add_script_component(selectedEntity, item.ID);
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return;
			}
		}

		ImGui::BeginChild("AddComponentList", ImVec2(0.0f, 0.0f), true);

		if (visible_total == 0) {
			ImGui::TextDisabled("No components match \"%s\".", m_add_component_query.c_str());
			ImGui::EndChild();
			ImGui::EndPopup();
			return;
		}

		if (visible_available_native > 0) {
			ImGui::SeparatorText("Native Components");
			for (const auto& item : available_native) {
				if (!text_matches_query(item.Name, m_add_component_query))
					continue;

				ImGui::PushID(item.Name);
				if (draw_picker_row(item.Name, "Native", true)) {
					activeScene->add_native_component(selectedEntity, item.Type);
					ImGui::CloseCurrentPopup();
					component_added = true;
					ImGui::PopID();
					break;
				}
				ImGui::PopID();
			}
		}

		if (visible_script > 0 && !component_added) {
			ImGui::SeparatorText("Script Components");
			for (const auto& item : available_script) {
				if (!text_matches_query(item.Name, m_add_component_query))
					continue;

				ImGui::PushID(item.Name);
				if (draw_picker_row(item.Name, "Script", true)) {
					activeScene->add_script_component(selectedEntity, item.ID);
					ImGui::CloseCurrentPopup();
					component_added = true;
					ImGui::PopID();
					break;
				}
				ImGui::PopID();
			}
		}

		if (visible_unavailable_native > 0 && !component_added) {
			ImGui::SeparatorText("Unavailable");
			ImGui::TextDisabled("Registered in the inspector, but not addable in the scene:");
			for (const auto& item : unavailable_native) {
				if (!text_matches_query(item.Name, m_add_component_query))
					continue;

				ImGui::PushID(item.Name);
				draw_picker_row(item.Name, "Missing Registry", false);
				ImGui::PopID();
			}
		}

		ImGui::EndChild();
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

	ImGui::EndDragDropTarget();
}

}
