#include <optional>

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include "components/EditorOutline.h"

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/EditorHiddenComponent.h"
#include "origo/components/MeshRenderer.h"
#include "origo/components/Name.h"

#include "panels/HierarchyPanel.h"

#include "state/EditorContext.h"

#include "systems/EditorIcons.h"

#include "ui/EditorNotificationSystem.h"

namespace OrigoEditor {

namespace {
bool entity_matches_search(const std::string& query, const std::string& entity_name) {
	if (query.empty())
		return true;

	std::string lowered_query;
	lowered_query.reserve(query.size());
	for (unsigned char c : query)
		lowered_query.push_back((char)std::tolower(c));

	std::string lowered_name;
	lowered_name.reserve(entity_name.size());
	for (unsigned char c : entity_name)
		lowered_name.push_back((char)std::tolower(c));

	return lowered_name.contains(lowered_query);
}

void duplicate_selected_entity(EditorContext& context, const Origo::RID& entity_id) {
	auto duplicate = context.ActiveScene->duplicate_entity(entity_id);
	if (!duplicate)
		return;

	context.set_selected_entity(*duplicate);
	EditorNotificationSystem::get_instance().success(
	    "Entity Duplicated",
	    "Added a duplicate to the scene.");
}
}

HierarchyPanel::HierarchyPanel(EditorContext& ctx)
    : m_context(ctx) { }

void HierarchyPanel::on_im_gui_render() {
	ImGui::TextUnformatted("Hierarchy");
	ImGui::Separator();
	ImGui::InputTextWithHint("##HierarchySearch", "Search entities...", &m_search_query);

	std::size_t total_entities = 0;
	std::size_t visible_entities = 0;
	for (const auto& entity_id : m_context.ActiveScene->get_entities()) {
		if (m_context.ActiveScene->has_native_component<Origo::EditorHiddenComponent>(entity_id))
			continue;

		++total_entities;

		auto* name_comp = m_context.ActiveScene->get_native_component<Origo::NameComponent>(entity_id);
		if (name_comp && entity_matches_search(m_search_query, name_comp->Name))
			++visible_entities;
	}

	ImGui::Text("%zu / %zu entities", visible_entities, total_entities);
	ImGui::Separator();

	auto selected_entity_id { m_context.get_selected_entity() };
	ImTextureID entity_icon { EditorIcons::get_instance().get(IconType::Entity) };

	ImGui::BeginChild("HierarchyList", ImVec2(0, 0), false);

	if (ImGui::BeginPopupContextWindow("HierarchyCreatePopup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
		ImGui::MenuItem("Empty");

		if (ImGui::IsItemClicked()) {
			auto ent { m_context.ActiveScene->create_entity("Entity") };
			m_context.ActiveScene->add_native_component<EditorOutlineComponent>(ent);
			EditorNotificationSystem::get_instance().success(
			    "Entity Created",
			    "Added Entity to the scene.");
		}

		ImGui::Separator();

		ImGui::MenuItem("Cube");
		if (ImGui::IsItemClicked()) {
			auto cube { m_context.ActiveScene->create_entity("Cube") };
			m_context.ActiveScene->add_native_component<EditorOutlineComponent>(cube);
			auto mesh_renderer { m_context.ActiveScene->add_native_component<Origo::MeshRendererComponent>(cube) };
			mesh_renderer->MeshHandle = Origo::PrimitiveShapeCache::get_instance().get_cube_mesh();
			mesh_renderer->MaterialHandle = Origo::DefaultAssetCache::get_instance().get_material();
			EditorNotificationSystem::get_instance().success(
			    "Entity Created",
			    "Added Cube to the scene.");
		}

		ImGui::MenuItem("Cone");
		if (ImGui::IsItemClicked()) {
			auto cone { m_context.ActiveScene->create_entity("Cone") };
			m_context.ActiveScene->add_native_component<EditorOutlineComponent>(cone);
			auto mesh_renderer { m_context.ActiveScene->add_native_component<Origo::MeshRendererComponent>(cone) };
			mesh_renderer->MeshHandle = Origo::PrimitiveShapeCache::get_instance().get_cone_mesh();
			mesh_renderer->MaterialHandle = Origo::DefaultAssetCache::get_instance().get_material();
			EditorNotificationSystem::get_instance().success(
			    "Entity Created",
			    "Added Cone to the scene.");
		}

		ImGui::MenuItem("Sphere");
		if (ImGui::IsItemClicked()) {
			auto sphere { m_context.ActiveScene->create_entity("Sphere") };
			m_context.ActiveScene->add_native_component<EditorOutlineComponent>(sphere);
			auto mesh_renderer { m_context.ActiveScene->add_native_component<Origo::MeshRendererComponent>(sphere) };
			mesh_renderer->MeshHandle = Origo::PrimitiveShapeCache::get_instance().get_sphere_mesh();
			mesh_renderer->MaterialHandle = Origo::DefaultAssetCache::get_instance().get_material();
			EditorNotificationSystem::get_instance().success(
			    "Entity Created",
			    "Added Sphere to the scene.");
		}

		ImGui::EndPopup();
	}

	for (const auto& entity_id : m_context.ActiveScene->get_entities()) {
		if (m_context.ActiveScene->has_native_component<Origo::EditorHiddenComponent>(entity_id)) {
			continue;
		}

		auto* name_comp = m_context.ActiveScene->get_native_component<Origo::NameComponent>(entity_id);
		if (!name_comp || !entity_matches_search(m_search_query, name_comp->Name)) {
			continue;
		}

		ImGui::PushID(entity_id.get_id());

		bool selected = selected_entity_id && selected_entity_id == entity_id;

		ImGui::Selectable("##EntitySelectable", selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !selected)
			m_context.set_selected_entity(entity_id);

		if (ImGui::BeginPopupContextItem("EntityContextPopup")) {
			if (ImGui::MenuItem("Duplicate", "Ctrl+D")) {
				duplicate_selected_entity(m_context, entity_id);
			}

			ImGui::MenuItem("Remove");
			if (ImGui::IsItemClicked()) {
				const std::string entity_name = name_comp->Name;
				m_context.ActiveScene->schedule_remove_entity(entity_id);
				m_context.unselect_entity();
				EditorNotificationSystem::get_instance().info(
				    "Entity Removed",
				    "Removed " + entity_name + " from the scene.");
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (entity_icon)
			ImGui::Image(entity_icon, ImVec2(16, 16));

		ImGui::SameLine(0.0f, 6.0f);

		ImGui::TextUnformatted(name_comp->Name.c_str());

		ImGui::PopID();
	}

	ImGui::EndChild();
}

}
