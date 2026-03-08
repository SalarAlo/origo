#pragma once

#include "origo/renderer/RenderView.h"

namespace Origo {

class GridRenderer {
public:
	~GridRenderer();

	void initialize();
	void render(const RenderView& view) const;

private:
	GLuint m_program {};
	GLuint m_fullscreen_vao {};
};

}
