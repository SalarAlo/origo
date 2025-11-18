#pragma once

#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/Renderer.h"

namespace Origo {

class RenderContext {
public:
	RenderContext(FrameBuffer* buffer = nullptr)
	    : m_Buffer(buffer)
	    , m_DrawQueue() {
	}

	void SetTarget(FrameBuffer* buffer) { m_Buffer = buffer; };

	void BeginFrame();
	void EndFrame();

	void Submit(Mesh* mesh, Material* material, Transform* transform);
	void Flush(Camera* camera);

private:
	FrameBuffer* m_Buffer;
	std::vector<RenderCommand> m_DrawQueue {};
};

}
