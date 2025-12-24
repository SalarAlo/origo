#pragma once

#include "state/EditorContext.h"

namespace OrigoEditor {

class EditorViewportController {
public:
	explicit EditorViewportController(EditorContext& ctx);

	Origo::Camera* GetActiveCamera();

private:
	EditorContext& m_Context;
};

}
