#pragma once

#include "state/EditorContext.h"
#include "EditorPanel.h"
#include "origo/assets/Texture.h"

namespace OrigoEditor {

class HierarchyPanel : public EditorPanel {
public:
	HierarchyPanel(EditorContext& ctx);

	const char* GetName() const override { return "Hierarchy"; }

	void OnImGuiRender() override;

private:
	void ChangeActiveSelectedEntity(Origo::Entity& e);

private:
	EditorContext& m_Context;
	Origo::Ref<Origo::Texture> m_EntityTex;
};

}
