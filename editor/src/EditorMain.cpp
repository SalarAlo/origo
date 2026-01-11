#include "layers/EditorCameraLayer.h"
#include "layers/LayerType.h"
#include "layers/RenderLayer.h"
#include "layers/SceneLayer.h"
#include "layers/UpdateLayer.h"
#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"
#include "origo/renderer/FrameBuffer.h"

#include "layers/EditorUILayer.h"
#include "state/EditorContext.h"
#include "systems/EditorIcons.h"
#include "systems/EditorRuntimeController.h"
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
	    , m_Context(new Scene("Sample Scene"), m_RenderBuffer, m_ResolveBuffer, m_Window.GetNativeWindow(), GetDefaultEditorPalette(), m_LayerSystem)
	    , m_RuntimeController(m_Context) {
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

	void OnEndFrame() override {

		if (m_Context.ActiveScene)
			m_Context.ActiveScene->EndFrame();

		if (!m_Context.PendingScene)
			return;

		m_Context.UnselectEntity();

		if (m_Context.RuntimeState == EditorRuntimeState::Running) {
			m_RuntimeController.Stop();
		}

		m_Context.EditorScene = std::move(m_Context.PendingScene);

		m_Context.ActiveScene = m_Context.EditorScene.get();
		m_Context.RuntimeState = EditorRuntimeState::Editing;
		m_Context.ViewMode = EditorViewMode::Editor;

		m_Context.RuntimeScene.reset();

		m_Context.UnselectEntity();
	}

private:
	FrameBuffer m_RenderBuffer;
	FrameBuffer m_ResolveBuffer;
	EditorContext m_Context;
	EditorRuntimeController m_RuntimeController;

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
