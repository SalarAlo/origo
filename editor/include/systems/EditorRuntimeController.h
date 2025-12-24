#pragma once

#include "state/EditorContext.h"
namespace OrigoEditor {

class EditorRuntimeController {
public:
	EditorRuntimeController(EditorContext& ctx)
	    : m_Context(ctx) { }

	void Play();
	void Stop();

private:
	EditorContext& m_Context;
};

}
