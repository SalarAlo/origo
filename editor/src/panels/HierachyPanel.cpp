#include "EditorContext.h"
#include "components/EditorMeshRenderer.h"
#include "panels/HierarchyPanel.h"

#include "origo/scene/Transform.h"

namespace OrigoEditor {
static Origo::Ref<Origo::Texture> LoadSVGTexture(const std::string& path, int size = 18) {
	auto texture = Origo::MakeRef<Origo::Texture>(Origo::TextureType::Albedo);
	texture->SetSource(Origo::MakeScope<Origo::TextureSourceSVG>(path, size, size));
	texture->LoadCpuIfTextureNotExistent();

	return texture;
}

HierarchyPanel::HierarchyPanel(EditorContext& ctx)
    : m_Context(ctx) {
	m_EntityTex = LoadSVGTexture("./icons/Entity.svg");
}

void HierarchyPanel::OnImGuiRender() {
	Origo::Entity* clickedEntity = nullptr;

	ImGui::Text("Scene Entities:");

	for (const auto& transform : m_Context.Scene.GetAllComponentsOfType<Origo::Transform>()) {
		auto& entity = *transform->AttachedTo;

		bool selected = m_Context.SelectedEntity.has_value() && m_Context.SelectedEntity->GetId() == entity.GetId();

		ImGui::Image(
		    (ImTextureID)(intptr_t)m_EntityTex->GetRendererID(),
		    ImVec2(16, 16));
		ImGui::SameLine();

		if (ImGui::Selectable(entity.GetName().c_str(), selected)) {
			clickedEntity = &entity;
		}
	}

	if (clickedEntity) {
		ChangeActiveSelectedEntity(*clickedEntity);
	}
}
void HierarchyPanel::ChangeActiveSelectedEntity(Origo::Entity& e) {
	if (m_Context.SelectedEntity.has_value()) {
		auto emr = m_Context.Scene.GetComponent<EditorMeshRenderer>(m_Context.SelectedEntity.value().GetId());
		emr->IsSelected = false;
	}

	m_Context.SelectedEntity = e;
	auto newEmr = m_Context.Scene.GetComponent<EditorMeshRenderer>(m_Context.SelectedEntity->GetId());
	newEmr->IsSelected = true;
}

}
