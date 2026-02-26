#include "origo/core/Application.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/core/Init.h"
#include "origo/core/Layer.h"
#include "origo/core/Logger.h"
#include "origo/core/Time.h"

#include "origo/input/Input.h"

namespace Origo {

Application::Application(const ApplicationSettings& settings)
    : m_settings(settings)
    , m_window(settings.WindowSettings)
    , m_running(true)
    , m_last_time_stamp(Time::get_now()) {
	m_window.set_event_callback(std::bind(&Application::on_event, this, std::placeholders::_1));
}

void Application::push_layer(Layer* layer, size_t key, bool frozen) {
	m_layer_system.request_push_layer(layer, key, frozen);
}

void Application::internal_update(double dt) {
	m_layer_system.update_all(dt);
}

void Application::internal_awake() {
	std::filesystem::current_path(m_settings.WorkingDirectory);

	Origo::init();

	Origo::Input::set_context(&m_window);

	m_layer_system.flush_commands();
	for (Layer* layer : m_layer_system) {
		layer->on_attach();
	}
}

void Application::internal_shutdown() {
	Logger::Shutdown();
	for (Layer* layer : m_layer_system) {
		layer->on_detach();
	}
	AssetSerializationSystem::cleanup();
}

void Application::on_event(Event& event) {
	for (auto it = m_layer_system.end(); it != m_layer_system.begin();) {
		(*--it)->on_event(event);
		if (event.is_handled())
			break;
	}
}

void Application::run() {
	internal_awake();
	on_awake();

	m_last_time_stamp = Time::get_now();

	while (m_running) {
		const auto now = Time::get_now();
		const double dt = std::chrono::duration<double>(now - m_last_time_stamp).count();
		m_last_time_stamp = now;

		internal_update(dt);

		if (m_window.should_close()) {
			m_running = false;
		}

		m_window.on_update();

		on_end_frame(dt);
	}

	internal_shutdown();
	on_shutdown();
}
}
