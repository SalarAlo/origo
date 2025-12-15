#pragma once

#include "origo/Camera.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/RenderCommand.h"
#include "origo/scene/Transform.h"

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

	void Submit(const AssetHandle& mesh, const AssetHandle& material, Transform* transform);
	void Flush(Camera* camera);

	void SetDrawMethod(GLenum drawMethod) { m_DrawMethod = drawMethod; }

private:
	FrameBuffer* m_Buffer {};
	std::vector<RenderCommand> m_DrawQueue {};
	GLenum m_DrawMethod { GL_TRIANGLES };
};

}
