#include "state/EditorContext.h"
#include "components/EditorSelection.h"
#include "panels/HierarchyPanel.h"

namespace OrigoEditor {
static Origo::Ref<Origo::Texture2D> LoadSVGTexture(const std::string& path, int size = 18) {
	auto texture = Origo::MakeRef<Origo::Texture2D>(Origo::TextureType::Albedo);
	texture->SetSource(Origo::MakeScope<Origo::TextureSourceSVG>(path, size, size));
	texture->Load();

	return texture;
}

HierarchyPanel::HierarchyPanel(EditorContext& ctx)
    : m_Context(ctx) {
	m_EntityTex = LoadSVGTexture("./icons/Entity.svg");
}

void HierarchyPanel::OnImGuiRender() {

	Origo::Entity* clickedEntity = nullptr;

	ImGui::Text("Scene Entities:");

	for (const auto& [id, entity] : m_Context.ActiveScene->GetEntities()) {
		bool selected = m_Context.SelectedEntity.has_value() && m_Context.SelectedEntity->GetID() == entity->GetID();

		ImGui::Image(
		    (ImTextureID)(intptr_t)m_EntityTex->GetRendererID(),
		    ImVec2(16, 16));
		ImGui::SameLine();

		if (ImGui::Selectable(entity->GetName().c_str(), selected)) {
			clickedEntity = entity;
		}
	}

	if (clickedEntity) {
		ChangeActiveSelectedEntity(*clickedEntity);
	}
}
void HierarchyPanel::ChangeActiveSelectedEntity(Origo::Entity& e) {
	if (m_Context.SelectedEntity.has_value()) {
		auto emr = m_Context.EditorScene->GetComponent<EditorSelection>(m_Context.SelectedEntity.value().GetID());
		if (emr)
			emr->IsSelected = false;
	}

	m_Context.SelectedEntity = e;
	auto newEmr = m_Context.EditorScene->GetComponent<EditorSelection>(m_Context.SelectedEntity->GetID());
	if (newEmr)
		newEmr->IsSelected = true;
}

}
