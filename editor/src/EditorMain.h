#pragma once

#include "origo/core/Application.h"

#include "origo/imgui/ImGuiLayer.h"

#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/RenderContext.h"

#include "state/EditorContext.h"

#include "systems/EditorRuntimeController.h"

namespace OrigoEditor {

class EditorApplication : public Origo::Application {
public:
	explicit EditorApplication(const Origo::ApplicationSettings& settings);

	void on_awake() override;
	void on_end_frame(float dt) override;

private:
	void setup_layers();
	void setup_render_context();

private:
	Origo::FrameBuffer m_render_buffer;
	Origo::FrameBuffer m_resolve_buffer;
	Origo::FrameBuffer m_game_render_buffer;
	Origo::FrameBuffer m_game_resolve_buffer;
	EditorContext m_context;
	EditorRuntimeController m_runtime_controller;

	Origo::RenderContext m_render_context { nullptr };
	Origo::ImGuiLayer m_im_gui_controller;
};

}
