#include "engine/FrameBuffer.h"

FrameBuffer::FrameBuffer(float pixelScale, ScreenWindowSettings settings) {
	glGenFramebuffers(0, &m_BufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferId);
}
