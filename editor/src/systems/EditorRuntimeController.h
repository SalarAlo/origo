#pragma once

#include "state/EditorContext.h"

namespace OrigoEditor {

class EditorRuntimeController {
public:
	EditorRuntimeController(EditorContext& ctx)
	    : m_context(ctx) { }

	void play();
	void pause(bool changeToEditorView = true);
	void stop();
	void resume();
	void step();

	bool can_step() const;
	bool can_resume() const;
	bool can_play() const;
	bool can_stop() const;
	bool can_pause() const;

private:
	EditorContext& m_context;
};

}
