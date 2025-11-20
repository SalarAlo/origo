#pragma once

#include "origo/Camera.h"
#include "origo/core/RID.h"
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

	void Submit(const RID& mesh, const RID& material, Transform* transform);
	void Flush(Camera* camera);

private:
	FrameBuffer* m_Buffer;
	std::vector<RenderCommand> m_DrawQueue {};
};

}
