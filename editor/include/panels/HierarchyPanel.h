#pragma once

#include "state/EditorContext.h"
#include "EditorPanel.h"
#include "origo/assets/Texture2D.h"

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
	Origo::Ref<Origo::Texture2D> m_EntityTex;
};

}
