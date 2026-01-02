#include "origo/scene/Name.h"
#include "state/EditorContext.h"
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
	ImGui::Text("Scene Entities:");

	auto selectedEntityID = m_Context.GetSelectedEntity();
	for (const auto& entityID : m_Context.ActiveScene->GetEntities()) {
		bool selected = selectedEntityID && selectedEntityID == entityID;

		auto entityImgID { (ImTextureID)(intptr_t)m_EntityTex->GetRendererID() };
		ImGui::Image(entityImgID, ImVec2(16, 16));
		ImGui::SameLine();

		auto nameComp { m_Context.ActiveScene->GetNativeComponent<Origo::Name>(entityID) };

		if (ImGui::Selectable(nameComp->GetName().c_str(), selected) && selectedEntityID != entityID) {
			m_Context.SetSelectedEntity(entityID);
		}
	}
}

}
