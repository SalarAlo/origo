#include "origo/core/Application.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/core/Layer.h"
#include "origo/core/Logger.h"
#include "origo/core/Time.h"
#include "origo/input/Input.h"
#include "origo/core/Init.h"

namespace Origo {
void Application::PushLayer(Layer* layer, size_t key, bool frozen) {
	m_LayerSystem.RequestPushLayer(layer, key, frozen);
}

void Application::InternalUpdate(double dt) {
	m_LayerSystem.UpdateAll(dt);
}

void Application::InternalAwake() {
	std::filesystem::current_path(m_Settings.WorkingDirectory);

	Origo::Init();

	Origo::Input::SetContext(&m_Window);

	m_LayerSystem.FlushCommands();
	for (Layer* layer : m_LayerSystem) {
		layer->OnAttach();
	}
}

void Application::InternalShutdown() {
	Logger::Shutdown();
	for (Layer* layer : m_LayerSystem) {
		layer->OnDetach();
	}
	AssetSerializationSystem::Cleanup();
}

Application::Application(const ApplicationSettings& settings)
    : m_Settings(settings)
    , m_Window(settings.WindowSettings)
    , m_Running(true)
    , m_LastTimeStamp(Time::GetNow()) {
	m_Window.SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
}

void Application::OnEvent(Event& event) {
	for (auto it = m_LayerSystem.end(); it != m_LayerSystem.begin();) {
		(*--it)->OnEvent(event);
		if (event.IsHandled())
			break;
	}
}

void Application::Run() {
	InternalAwake();
	OnAwake();

	m_LastTimeStamp = Time::GetNow();

	while (m_Running) {
		double dt { static_cast<Time::Duration>(Time::GetNow() - m_LastTimeStamp).count() };

		InternalUpdate(dt);

		if (m_Window.ShouldClose()) {
			m_Running = false;
		}

		m_LastTimeStamp = Time::GetNow();

		m_Window.OnUpdate();
	}

	InternalShutdown();
	OnShutdown();
}
}
