#pragma once

#include "origo/renderer/FrameBuffer.h"
#include "origo/Camera.h"
#include "panels/EditorPanel.h"

namespace OrigoEditor {

class SceneViewport : public EditorPanel {
public:
	SceneViewport(Origo::FrameBuffer* renderTarget, Origo::FrameBuffer* resolveTarget, Origo::Camera& camera)
	    : m_RenderTarget(renderTarget)
	    , m_ResolveTarget(resolveTarget)
	    , m_Camera(camera) {
	}

	void SetTargets(Origo::FrameBuffer* renderTarget, Origo::FrameBuffer* resolveTarget) {
		m_RenderTarget = renderTarget;
		m_ResolveTarget = resolveTarget;
	}

	void OnImGuiRender();

	const char* GetName() const { return "Viewport"; }

private:
	Origo::FrameBuffer* m_RenderTarget {};
	Origo::FrameBuffer* m_ResolveTarget {};
	Origo::Camera& m_Camera;
};

}
