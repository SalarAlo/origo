
#pragma once

#include "EditorPanel.h"

namespace OrigoEditor {

class RuntimeStatePanel : public EditorPanel {
public:
	const char* GetName() const override { return "RuntimePanel"; }

	void OnImGuiRender() override;

private:
};

}
