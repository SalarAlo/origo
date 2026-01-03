#pragma once

#include "state/EditorContext.h"
namespace OrigoEditor {

class EditorRuntimeController {
public:
	EditorRuntimeController(EditorContext& ctx)
	    : m_Context(ctx) { }

	void Play();
	void Pause(bool changeToEditorView = true);
	void Stop();
	void Resume();
	void Step();

	bool CanStep() const;
	bool CanResume() const;
	bool CanPlay() const;
	bool CanStop() const;
	bool CanPause() const;

private:
	EditorContext& m_Context;
};

}
