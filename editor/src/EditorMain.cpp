#include "layer/SceneLayer.h"
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
	    , m_RenderBuffer([] {
		    FrameBufferSpec spec;
		    spec.Width = 1920;
		    spec.Height = 1080;
		    spec.Samples = 4;
		    spec.Attachments = {
			    { AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
			    { AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
		    };
		    return spec;
	    }())
	    , m_ResolveBuffer([] {
		    FrameBufferSpec spec;
		    spec.Width = 1920;
		    spec.Height = 1080;
		    spec.Samples = 1;
		    spec.Attachments = {
			    { AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
			    { AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
		    };
		    return spec;
	    }())
	    , m_Context(m_Scene, m_RenderBuffer, m_ResolveBuffer, m_Window.GetNativeWindow()) {

		PushLayer(new EditorCameraLayer(m_Scene.GetMainCamera()));
		PushLayer(new SceneLayer(m_Context));
		PushLayer(new EditorUILayer(m_Context));

		m_RenderContext.SetTarget(&m_RenderBuffer);
		m_RenderContext.SetResolveTarget(&m_ResolveBuffer);
	}

private:
	FrameBuffer m_RenderBuffer;
	FrameBuffer m_ResolveBuffer;
	EditorContext m_Context;
};

}

namespace Origo {
Application* CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./editor/workspace",
		.WindowSettings = { .Width = 1900, .Height = 900, .Title = "Origo Editor" }
	};
	return new OrigoEditor::EditorApplication(settings);
}
}
