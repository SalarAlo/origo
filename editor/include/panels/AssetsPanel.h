#pragma once

#include "EditorPanel.h"

namespace OrigoEditor {

class AssetsPanel : public EditorPanel {
public:
	const char* GetName() const override { return "Assets"; }

	void OnImGuiRender() override;

private:
};

}
