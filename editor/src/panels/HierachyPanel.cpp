#include <optional>

#include "imgui.h"

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
	constexpr float confirmation_duration_seconds = 0.8f;
}

HierarchyPanel::HierarchyPanel(EditorContext& ctx)
    : m_context(ctx) { }

void HierarchyPanel::on_im_gui_render() {
	ImGui::TextUnformatted("Hierarchy");
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
			    "Added Entity to the scene.",
			    confirmation_duration_seconds);
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
			    "Added Cube to the scene.",
			    confirmation_duration_seconds);
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
			    "Added Cone to the scene.",
			    confirmation_duration_seconds);
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
			    "Added Sphere to the scene.",
			    confirmation_duration_seconds);
		}

		ImGui::EndPopup();
	}

	for (const auto& entity_id : m_context.ActiveScene->get_entities()) {
		if (m_context.ActiveScene->has_native_component<Origo::EditorHiddenComponent>(entity_id)) {
			continue;
		}

		auto* name_comp = m_context.ActiveScene->get_native_component<Origo::NameComponent>(entity_id);

		ImGui::PushID(entity_id.get_id());

		bool selected = selected_entity_id && selected_entity_id == entity_id;

		ImGui::Selectable("##EntitySelectable", selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !selected)
			m_context.set_selected_entity(entity_id);

		if (ImGui::BeginPopupContextItem("EntityContextPopup")) {
			ImGui::MenuItem("Remove");
			if (ImGui::IsItemClicked()) {
				const std::string entity_name = name_comp->Name;
				m_context.ActiveScene->schedule_remove_entity(entity_id);
				m_context.unselect_entity();
				EditorNotificationSystem::get_instance().info(
				    "Entity Removed",
				    "Removed " + entity_name + " from the scene.",
				    confirmation_duration_seconds);
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
