#include <optional>

#include "imgui.h"

#include "components/EditorOutline.h"

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/MeshRenderer.h"
#include "origo/components/Name.h"

#include "panels/HierarchyPanel.h"

#include "state/EditorContext.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {

HierarchyPanel::HierarchyPanel(EditorContext& ctx)
    : m_Context(ctx) { }

void HierarchyPanel::OnImGuiRender() {
	ImGui::TextUnformatted("Hierarchy");
	ImGui::Separator();

	auto selectedEntityID { m_Context.GetSelectedEntity() };
	ImTextureID entityIcon { EditorIcons::Get(IconType::Entity) };

	ImGui::BeginChild("HierarchyList", ImVec2(0, 0), false);

	if (ImGui::BeginPopupContextWindow("HierarchyCreatePopup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
		ImGui::MenuItem("Empty");
		if (ImGui::IsItemClicked()) {
			auto ent { m_Context.ActiveScene->CreateEntity("Entity") };
			m_Context.ActiveScene->AddNativeComponent<EditorOutlineComponent>(ent);
		}

		ImGui::Separator();

		ImGui::MenuItem("Cube");
		if (ImGui::IsItemClicked()) {
			auto cube { m_Context.ActiveScene->CreateEntity("Cube") };
			m_Context.ActiveScene->AddNativeComponent<EditorOutlineComponent>(cube);
			auto meshRenderer { m_Context.ActiveScene->AddNativeComponent<Origo::MeshRendererComponent>(cube) };
			meshRenderer->SetMesh(Origo::PrimitiveShapeCache::GetInstance().GetCubeMesh());
			meshRenderer->SetMaterial(Origo::DefaultAssetCache::GetInstance().GetMaterial());
		}

		ImGui::MenuItem("Sphere");
		if (ImGui::IsItemClicked()) {
			auto cube { m_Context.ActiveScene->CreateEntity("Sphere") };
			m_Context.ActiveScene->AddNativeComponent<EditorOutlineComponent>(cube);
			auto meshRenderer { m_Context.ActiveScene->AddNativeComponent<Origo::MeshRendererComponent>(cube) };
			meshRenderer->SetMesh(Origo::PrimitiveShapeCache::GetInstance().GetSphereMesh());
			meshRenderer->SetMaterial(Origo::DefaultAssetCache::GetInstance().GetMaterial());
		}

		ImGui::EndPopup();
	}

	for (const auto& entityID : m_Context.ActiveScene->GetEntities()) {
		ImGui::PushID(entityID.GetId());

		bool selected = selectedEntityID && selectedEntityID == entityID;

		ImGui::Selectable("##EntitySelectable", selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !selected)
			m_Context.SetSelectedEntity(entityID);

		if (ImGui::BeginPopupContextItem("EntityContextPopup")) {
			ImGui::MenuItem("Remove");
			if (ImGui::IsItemClicked()) {
				m_Context.ActiveScene->ScheduleRemoveEntity(entityID);
				m_Context.UnselectEntity();
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (entityIcon)
			ImGui::Image(entityIcon, ImVec2(16, 16));

		ImGui::SameLine(0.0f, 6.0f);

		auto* nameComp = m_Context.ActiveScene->GetNativeComponent<Origo::NameComponent>(entityID);
		ImGui::TextUnformatted(nameComp->GetName().c_str());

		ImGui::PopID();
	}

	ImGui::EndChild();
}

}
