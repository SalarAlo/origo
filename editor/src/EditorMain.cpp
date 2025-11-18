#include "layer/SceneLayer.h"
#include "origo/assets/AssetManager.h"
#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"
#include "origo/renderer/FrameBuffer.h"

#include "layer/EditorCameraLayer.h"

#include "layer/EditorUILayer.h"
#include "EditorContext.h"
#include "origo/scene/Scene.h"

using namespace Origo;

namespace OrigoEditor {

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationSettings& settings)
	    : Application(settings)
	    , m_Buffer([] {
		    FrameBufferSpec spec;
		    spec.Width = 1920;
		    spec.Height = 1080;
		    spec.Attachments = {
			    { AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
			    { AttachmentType::Color, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT },
			    { AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
		    };
		    return spec;
	    }())
	    , m_Context(m_Scene, m_Buffer, m_Window.GetNativeWindow()) {
		PushLayer(new EditorCameraLayer(m_Scene.GetMainCamera()));
		PushLayer(new SceneLayer(m_Context));
		PushLayer(new EditorUILayer(m_Context));

		m_RenderContext.SetTarget(&m_Buffer);
	}

private:
	FrameBuffer m_Buffer;
	EditorContext m_Context;
};

}

namespace Origo {
Application* CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./",
		.WindowSettings = { .Width = 1800, .Height = 1000, .Title = "ITSAFLOATER" }
	};
	return new OrigoEditor::EditorApplication(settings);
}
}
