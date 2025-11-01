#pragma once

#include "EditorPanel.h"
#include "origo/Camera.h"
#include "origo/renderer/FrameBuffer.h"

namespace OrigoEditor {

class SceneViewport : public EditorPanel {
public:
	SceneViewport(Origo::FrameBuffer& buffer, Origo::Ref<Origo::Camera> camera)
	    : m_FrameBuffer(buffer)
	    , m_Camera(camera) { }

	const char* GetName() const override { return "Scene Viewport"; }

	void OnImGuiRender() override;

private:
	Origo::FrameBuffer& m_FrameBuffer;
	Origo::Ref<Origo::Camera> m_Camera;
};

}
