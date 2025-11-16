#include "origo/core/Application.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/core/Layer.h"
#include "origo/core/Logger.h"
#include "origo/core/Time.h"
#include "origo/input/Input.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/ComponentSystemRegistry.h"
#include <functional>

namespace Origo {
void Application::PushLayer(Layer* l) {
	m_LayerStack.PushLayer(l);
}

void Application::PushOverlay(Layer* l) {
	m_LayerStack.PushOverlay(l);
}
void Application::InternalUpdate(double dt) {
	ComponentSystemRegistry::GetInstance().RunAll(m_Scene);

	for (Layer* layer : m_LayerStack) {
		layer->OnUpdate(dt);
	}
}

void Application::InternalAwake() {
	Origo::Logger::Init();
	Origo::Input::SetContext(&m_Window);

	for (Layer* layer : m_LayerStack) {
		layer->OnAttach();
	}
}

void Application::InternalShutdown() {
	Logger::Shutdown();
	for (Layer* layer : m_LayerStack) {
		layer->OnDetach();
	}
	AssetSerializationSystem::Cleanup();
}

Application::Application(const ApplicationSettings& settings)
    : m_Settings(settings)
    , m_Window(settings.WindowSettings)
    , m_Running(true)
    , m_LastTimeStamp(Time::GetNow())
    , m_Scene("Origo Game Sample", m_Window.GetAspectResolution()) {
	m_Window.SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
}

void Application::OnEvent(Event& event) {
	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
		(*--it)->OnEvent(event);
		if (event.IsHandled())
			break;
	}
}

void Application::Run() {
	InternalAwake();
	OnAwake();

	while (m_Running) {
		double dt = static_cast<Time::Duration>(Time::GetNow() - m_LastTimeStamp).count();

		Renderer::BeginFrame();

		InternalUpdate(dt);

		Renderer::Flush(m_Scene.GetMainCamera());

		Renderer::EndFrame();
		if (m_Window.ShouldClose()) {
			m_Running = false;
		}

		m_LastTimeStamp = Time::GetNow();

		m_Window.OnUpdate();
	}

	InternalShutdown();
}

}
