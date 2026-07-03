#include <algorithm>

#include "InspectorComponentRenderer.h"

#include "imgui.h"

#include "origo/assets/Texture2D.h"

#include "origo/components/NameComponent.h"
#include "origo/core/PathContext.h"

#include "origo/scripting/ComponentRegistry.h"
#include "origo/scripting/ScriptRegistry.h"

#include "state/EditorContext.h"

#include "systems/EditorIcons.h"

#include "ui/ComponentUI.h"

namespace OrigoEditor {

void InspectorComponentRenderer::init(EditorContext* ctx) {
	m_context = ctx;
}

void InspectorComponentRenderer::draw_native_component(const Origo::RID& entity, void* componentPtr, std::type_index type) {
	auto* entry = InspectorDrawRegistry::get_instance().get_inspector_entry(type);

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

void InspectorComponentRenderer::draw_reflected_component(const Origo::RID& entity, Origo::ComponentInstanceData& instance) {
	if (!Origo::ComponentRegistry::exists(instance.ID))
		return;

	const auto& desc = Origo::ComponentRegistry::get(instance.ID);
	if (!m_script_icon)
		m_script_icon = load_icon("./icons/Script.svg");

	std::string label = desc.Name + "##reflected_component" + std::to_string(reinterpret_cast<uintptr_t>(&instance));
	if (!draw_header_reflected(entity, &instance, label.c_str(), m_script_icon, false, true))
		return;

	for (auto& field : instance.Fields)
		draw_reflected_field(field);
}

void InspectorComponentRenderer::draw_reflected_script(const Origo::RID& entity, Origo::ScriptAttachmentData& attachment) {
	if (!Origo::ScriptRegistry::exists(attachment.ID))
		return;

	const auto& desc = Origo::ScriptRegistry::get(attachment.ID);
	if (!m_script_icon)
		m_script_icon = load_icon("./icons/Script.svg");

	std::string label = desc.Name + "##reflected_script" + std::to_string(reinterpret_cast<uintptr_t>(&attachment));
	const bool removable = desc.ComponentType.Type == Origo::UUID::bad();
	if (!draw_header_reflected(entity, &attachment, label.c_str(), m_script_icon, true, removable))
		return;

	if (desc.ComponentType.Type != Origo::UUID::bad())
		return;

	for (auto& field : attachment.Fields)
		draw_reflected_field(field);
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
				m_context->ActiveScene->schedule_remove_native_component(entity, type);
			}
			ImGui::EndPopup();
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
	return open;
}

bool InspectorComponentRenderer::draw_header_reflected(const Origo::RID& entity, void* id, const char* label, Origo::Ref<Origo::Texture2D> icon, bool behavior_script, bool removable) {
	ImGui::PushID(id);

	bool open = false;

	if (ImGui::BeginTable("##reflected_inspector_row", 2, ImGuiTableFlags_SizingFixedFit)) {
		ImGui::TableSetupColumn("##icon", ImGuiTableColumnFlags_WidthFixed, 24.0f);
		ImGui::TableSetupColumn("##header", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		const float icon_size = 18.0f;
		const float y_off = (ImGui::GetFrameHeight() - icon_size) * 0.5f;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y_off);

		if (icon)
			ImGui::Image((ImTextureID)(intptr_t)icon->get_renderer_id(), ImVec2(icon_size, icon_size));

		ImGui::TableSetColumnIndex(1);
		open = ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
		ImGui::SetItemAllowOverlap();

		ImRect cell_rect = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 1);
		const float menu_size = 14.0f;
		const float x = cell_rect.Max.x - menu_size - ImGui::GetStyle().CellPadding.x - ImGui::GetStyle().FramePadding.x;
		ImRect header_rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		const float y = header_rect.Min.y + (header_rect.GetHeight() - menu_size) * 0.5f;

		ImGui::SetCursorScreenPos(ImVec2(x, y));
		ImGui::SetNextItemAllowOverlap();

		ImTextureID menu_icon = EditorIcons::get_instance().get(IconType::Menu);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));

		if (ImGui::ImageButton("##reflected_menu_btn", menu_icon, ImVec2(menu_size, menu_size)))
			ImGui::OpenPopup("##reflected_menu");

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);

		if (ImGui::BeginPopup("##reflected_menu")) {
			if (!removable)
				ImGui::BeginDisabled();
			if (ImGui::MenuItem(behavior_script ? "Remove Script" : "Remove Component") && m_context && m_context->ActiveScene) {
				if (behavior_script)
					m_context->ActiveScene->remove_script_if_exists(entity, static_cast<Origo::ScriptAttachmentData*>(id)->ID);
				else
					m_context->ActiveScene->remove_component_data_if_exists(entity, static_cast<Origo::ComponentInstanceData*>(id)->ID);
				open = false;
			}
			if (!removable)
				ImGui::EndDisabled();
			ImGui::EndPopup();
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
	return open;
}

void InspectorComponentRenderer::draw_unknown(void* componentPtr) {
	if (!m_unknown_icon)
		m_unknown_icon = load_icon("./icons/Entity.svg");

	std::string label = "Unknown Component##" + std::to_string(reinterpret_cast<uintptr_t>(componentPtr));

	ImGui::TextDisabled("(No inspector drawer registered)");
}

void InspectorComponentRenderer::draw_reflected_field(Origo::ReflectedFieldValue& field) {
	ImGui::PushID(field.ID.to_string().c_str());

	switch (field.Value.get_type()) {
	case Origo::VariantType::Bool: {
		bool v {};
		field.Value.TryGetAsBool(v);
		ComponentUI::draw_bool_control(field.Name, v);
		field.Value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::Int: {
		int v {};
		field.Value.TryGetAsInt(v);
		ComponentUI::draw_int_control(field.Name, v);
		field.Value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::Float: {
		float v {};
		field.Value.TryGetAsFloat(v);
		ComponentUI::draw_float_control(field.Name, v);
		field.Value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::String: {
		std::string v;
		field.Value.TryGetAsString(v);
		ComponentUI::draw_string_control(field.Name, v);
		field.Value = Origo::Variant(v);
		break;
	}
	case Origo::VariantType::Enum:
	case Origo::VariantType::Vec2:
	case Origo::VariantType::Vec3:
	case Origo::VariantType::Vec4:
		ImGui::TextDisabled("%s (unsupported reflected field)", field.Name.c_str());
		break;
	case Origo::VariantType::Entity:
		draw_entity_ref_field(field);
		break;
	case Origo::VariantType::Asset:
	case Origo::VariantType::Struct:
		ImGui::TextDisabled("%s (unsupported reflected field)", field.Name.c_str());
		break;
	}

	ImGui::PopID();
}

void InspectorComponentRenderer::draw_entity_ref_field(Origo::ReflectedFieldValue& field) {
	constexpr float field_width = 180.0f;
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(field.Name.c_str());
	ImGui::SameLine(std::max(
	    ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - field_width,
	    ImGui::GetCursorPosX() + ImGui::CalcTextSize(field.Name.c_str()).x + 8.0f));

	Origo::RID entity {};
	field.Value.TryGetAsEntity(entity);

	std::string label = "None";
	bool valid = false;
	if (m_context && m_context->ActiveScene && entity.get_id() >= 0) {
		valid = m_context->ActiveScene->has_entity(entity);
		if (valid) {
			if (auto* name = m_context->ActiveScene->get_native_component<Origo::NameComponent>(entity))
				label = name->Name;
			else
				label = "Entity " + entity.to_string();
		} else {
			label = "Missing Entity " + entity.to_string();
		}
	}

	ImGui::SetNextItemWidth(field_width - 28.0f);
	ImGui::Button(label.c_str(), ImVec2(field_width - 28.0f, 0.0f));

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ORIGO_ENTITY_ID")) {
			const char* id = static_cast<const char*>(payload->Data);
			try {
				const Origo::RID dropped = Origo::RID::from_string(id);
				if (m_context && m_context->ActiveScene && m_context->ActiveScene->has_entity(dropped))
					field.Value = Origo::Variant(dropped);
			} catch (const std::exception&) {
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (!valid && entity.get_id() >= 0 && ImGui::IsItemHovered())
		ImGui::SetTooltip("Referenced entity no longer exists.");

	ImGui::SameLine(0.0f, 4.0f);
	if (ImGui::Button("X", ImVec2(24.0f, 0.0f)))
		field.Value = Origo::Variant(Origo::RID {});
}

Origo::Ref<Origo::Texture2D> InspectorComponentRenderer::load_icon(const std::string& path) {
	auto tex = Origo::make_ref<Origo::Texture2D>(
	    Origo::TextureType::UI);

	tex->set_source(
	    Origo::make_scope<Origo::TextureSourceSVG>(
	        Origo::PathContextService::get_instance().editor().resolve(path).string(),
	        14,
	        14));

	tex->load();
	return tex;
}
}
