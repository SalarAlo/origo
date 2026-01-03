#pragma once

#include "EditorPanel.h"
#include "origo/core/UUID.h"
#include <spdlog/spdlog.h>

namespace OrigoEditor {

class AssetPanel : public EditorPanel {
public:
	const char* GetName() const override { return "Assets"; }
	void OnImGuiRender() override;

private:
	Origo::UUID selectedAssetID;
};

}
