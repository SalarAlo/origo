
#pragma once

#include "EditorPanel.h"

namespace OrigoEditor {

class ConsolePanel : public EditorPanel {
public:
	const char* GetName() const override { return "ConsolePanel"; }

	void OnImGuiRender() override;

private:
};

}
