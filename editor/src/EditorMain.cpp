#include "layers/EditorCameraLayer.h"
#include "layers/EditorUILayer.h"
#include "layers/LayerType.h"
#include "layers/RenderLayer.h"
#include "layers/SceneLayer.h"
#include "layers/UpdateLayer.h"

#include "origo/core/Application.h"
#include "origo/core/EntryPoint.h"

#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/RenderContext.h"

#include "origo/scene/Scene.h"

#include "state/EditorContext.h"

#include "systems/EditorIcons.h"
#include "systems/EditorRuntimeController.h"

#include "ui/EditorPalette.h"

using namespace Origo;

namespace OrigoEditor {

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationSettings& settings)
	    : Application(settings)
	    , m_render_buffer([] {
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
	    , m_resolve_buffer([] {
		    FrameBufferSpec spec;
		    spec.Width = 1920;
		    spec.Attachments = {
			    { AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
			    { AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
		    };
		    return spec;
	    }())
	    , m_context(new Scene("Sample Scene"), m_render_buffer, m_resolve_buffer, m_window.get_native_window(), get_default_editor_palette(), m_layer_system)
	    , m_runtime_controller(m_context) {
		push_layer(new EditorCameraLayer(m_context), static_cast<size_t>(LayerType::EditorCameraLayer));
		push_layer(new SceneLayer(m_context), static_cast<size_t>(LayerType::SceneLayer));
		push_layer(new EditorUILayer(m_context, m_im_gui_controller), static_cast<size_t>(LayerType::EditorUILayer));
		push_layer(new RenderLayer(m_context, m_render_context), static_cast<size_t>(LayerType::RenderLayer));
		push_layer(new UpdateLayer(m_context, m_render_context), static_cast<size_t>(LayerType::UpdateLayer), true);

		m_render_context.set_target(&m_render_buffer);
		m_render_context.set_resolve_target(&m_resolve_buffer);
	}

	void on_awake() override {
		EditorIcons::get_instance().init();
	}

	void on_end_frame(float dt) override {
		m_context.DeltaTime = dt;

		if (m_context.ActiveScene)
			m_context.ActiveScene->end_frame();

		if (!m_context.PendingScene)
			return;

		m_context.unselect_entity();

		if (m_context.RuntimeState == EditorRuntimeState::Running) {
			m_runtime_controller.stop();
		}

		m_context.EditorScene = std::move(m_context.PendingScene);

		m_context.ActiveScene = m_context.EditorScene.get();
		m_context.RuntimeState = EditorRuntimeState::Editing;
		m_context.ViewMode = EditorViewMode::Editor;

		m_context.RuntimeScene.reset();

		m_context.unselect_entity();
	}

private:
	FrameBuffer m_render_buffer;
	FrameBuffer m_resolve_buffer;
	EditorContext m_context;
	EditorRuntimeController m_runtime_controller;

	RenderContext m_render_context { nullptr };
	Origo::ImGuiLayer m_im_gui_controller;
};

}

namespace Origo {
Application* create_application() {
	ApplicationSettings settings {
		.WorkingDirectory = "./editor/workspace",
		.WindowSettings = { .Width = 1900, .Height = 900, .Title = "Origo" }
	};
	return new OrigoEditor::EditorApplication(settings);
}

}
