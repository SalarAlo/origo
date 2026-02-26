#pragma once

#include "state/EditorContext.h"
#include "EditorPanel.h"

namespace OrigoEditor {

class AssetInspectorPanel : public EditorPanel {
public:
	AssetInspectorPanel(EditorContext& ctx)
	    : m_context(ctx) { }

	const char* get_name() const override { return "Asset Inspector"; }
	void on_im_gui_render() override;

private:
	EditorContext& m_context;
};

}
