#include "EditorMain.h"

#include "layers/EditorCameraLayer.h"
#include "layers/EditorUILayer.h"
#include "layers/LayerType.h"
#include "layers/RenderLayer.h"
#include "layers/SceneLayer.h"
#include "layers/UpdateLayer.h"

#include "origo/core/EntryPoint.h"

#include "systems/EditorIcons.h"

#include "ui/EditorPalette.h"

using namespace Origo;

namespace {

FrameBufferSpec create_render_buffer_spec() {
	FrameBufferSpec spec;
	spec.Width = 1920;
	spec.Height = 1080;
	spec.Samples = 4;
	spec.Attachments = {
		{ AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
		{ AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
	};
	return spec;
}

FrameBufferSpec create_resolve_buffer_spec() {
	FrameBufferSpec spec;
	spec.Width = 1920;
	spec.Height = 1080;
	spec.Attachments = {
		{ AttachmentType::Color, GL_RGBA16F, GL_RGBA, GL_FLOAT },
		{ AttachmentType::Depth, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT },
	};
	return spec;
}

size_t layer_key(OrigoEditor::LayerType layer_type) {
	return static_cast<size_t>(layer_type);
}

ApplicationSettings create_default_application_settings() {
	return {
		.WorkingDirectory = "./editor/workspace",
		.WindowSettings = { .Width = 1900, .Height = 900, .Title = "Origo" }
	};
}

}

namespace OrigoEditor {

EditorApplication::EditorApplication(const ApplicationSettings& settings)
    : Application(settings)
    , m_render_buffer(create_render_buffer_spec())
    , m_resolve_buffer(create_resolve_buffer_spec())
    , m_context(new Scene("Sample Scene"), m_render_buffer, m_resolve_buffer, m_window.get_native_window(), get_default_editor_palette(), m_layer_system)
    , m_runtime_controller(m_context) {
	setup_layers();
	setup_render_context();
}

void EditorApplication::on_awake() {
	EditorIcons::get_instance().init();
}

void EditorApplication::setup_layers() {
	push_layer(make_scope<EditorCameraLayer>(m_context), layer_key(LayerType::EditorCameraLayer));
	push_layer(make_scope<SceneLayer>(m_context), layer_key(LayerType::SceneLayer));
	push_layer(make_scope<EditorUILayer>(m_context, m_im_gui_controller), layer_key(LayerType::EditorUILayer));
	push_layer(make_scope<RenderLayer>(m_context, m_render_context), layer_key(LayerType::RenderLayer));
	push_layer(make_scope<UpdateLayer>(m_context, m_render_context), layer_key(LayerType::UpdateLayer), true);
}

void EditorApplication::setup_render_context() {
	m_render_context.set_target(&m_render_buffer);
	m_render_context.set_resolve_target(&m_resolve_buffer);
}

void EditorApplication::on_end_frame(float dt) {
	m_context.DeltaTime = dt;

	if (m_context.ActiveScene)
		m_context.ActiveScene->end_frame();

	if (!m_context.PendingScene)
		return;

	// TODO: still keep selection between runs
	m_context.unselect_entity();

	if (m_context.RuntimeState == EditorRuntimeState::Running)
		m_runtime_controller.stop();

	m_context.EditorScene = std::move(m_context.PendingScene);

	m_context.ActiveScene = m_context.EditorScene.get();
	m_context.RuntimeState = EditorRuntimeState::Editing;
	m_context.ViewMode = EditorViewMode::Editor;

	m_context.RuntimeScene.reset();

	m_context.unselect_entity();
}

}

namespace Origo {
Application* create_application() {
	return new OrigoEditor::EditorApplication(create_default_application_settings());
}

}
