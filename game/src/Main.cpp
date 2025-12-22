#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"
#include "origo/renderer/FrameBuffer.h"

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
			    { AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
		    };
		    return spec;
	    }()) {
		m_RenderContext.SetTarget(&m_Buffer);
	}

private:
	FrameBuffer m_Buffer;
};

}

namespace Origo {
Application* CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./game",
		.WindowSettings = { .Width = 1900, .Height = 900, .Title = "Origo Editor" }
	};
	return new OrigoEditor::EditorApplication(settings);
}

}
