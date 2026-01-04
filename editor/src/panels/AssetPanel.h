#pragma once

#include "EditorPanel.h"
#include "origo/core/UUID.h"
#include "state/EditorContext.h"
#include <spdlog/spdlog.h>

namespace OrigoEditor {

class AssetPanel : public EditorPanel {
public:
	AssetPanel(EditorContext& ctx)
	    : m_Context(ctx) { }
	const char* GetName() const override { return "Assets"; }
	void OnImGuiRender() override;

private:
	EditorContext& m_Context;
};

}
