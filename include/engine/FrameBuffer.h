#pragma once

#include "engine/ScreenWindow.h"

class FrameBuffer {
public:
	FrameBuffer(float pixelScale, ScreenWindowSettings settings);

private:
	GLuint m_BufferId;
};
