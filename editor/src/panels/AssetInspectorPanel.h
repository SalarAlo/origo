#pragma once

#include "state/EditorContext.h"
#include "EditorPanel.h"

namespace OrigoEditor {

class AssetInspectorPanel : public EditorPanel {
public:
	AssetInspectorPanel(EditorContext& ctx)
	    : m_Context(ctx) { }

	const char* GetName() const override { return "Asset Inspector"; }
	void OnImGuiRender() override;

private:
	EditorContext& m_Context;
};

}
