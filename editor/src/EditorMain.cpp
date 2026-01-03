#include "layer/EditorCameraLayer.h"
#include "layer/LayerType.h"
#include "layer/RenderLayer.h"
#include "layer/SceneLayer.h"
#include "layer/UpdateLayer.h"
#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"
#include "origo/renderer/FrameBuffer.h"

#include "layer/EditorUILayer.h"
#include "state/EditorContext.h"
#include "systems/EditorIcons.h"
#include "ui/EditorPalette.h"

#include "origo/renderer/RenderContext.h"
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
		    spec.Attachments = {
			    { AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
			    { AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
		    };
		    return spec;
	    }())
	    , m_Context(&m_Scene, m_RenderBuffer, m_ResolveBuffer, m_Window.GetNativeWindow(), GetDefaultEditorPalette(), m_LayerSystem)
	    , m_Scene("Sample Scene") {
		PushLayer(new EditorCameraLayer(m_Context), static_cast<size_t>(LayerType::EditorCameraLayer));
		PushLayer(new SceneLayer(m_Context), static_cast<size_t>(LayerType::SceneLayer));
		PushLayer(new EditorUILayer(m_Context, m_ImGuiController), static_cast<size_t>(LayerType::EditorUILayer));
		PushLayer(new RenderLayer(m_Context, m_RenderContext), static_cast<size_t>(LayerType::RenderLayer));
		PushLayer(new UpdateLayer(m_Context), static_cast<size_t>(LayerType::UpdateLayer), true);

		m_RenderContext.SetTarget(&m_RenderBuffer);
		m_RenderContext.SetResolveTarget(&m_ResolveBuffer);
	}

	void OnAwake() override {
		EditorIcons::Init();
	}

private:
	FrameBuffer m_RenderBuffer;
	FrameBuffer m_ResolveBuffer;
	EditorContext m_Context;

	Scene m_Scene;
	RenderContext m_RenderContext { nullptr };
	Origo::ImGuiLayer m_ImGuiController;
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
