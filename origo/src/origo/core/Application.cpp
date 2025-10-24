#include "origo/core/Application.h"
#include "origo/core/Logger.h"
#include "origo/core/Time.h"
#include "origo/input/Input.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/ComponentSystemRegistry.h"
#include <functional>

namespace Origo {
void Application::InternalUpdate() {
	ComponentSystemRegistry::GetInstance().RunAll(m_Scene);
}

Application::Application(const ApplicationSettings& settings)
    : m_Settings(settings)
    , m_Window(settings.WindowSettings)
    , m_Running(true)
    , m_LastTimeStamp(Time::GetNow())
    , m_Scene("Origo Game Sample", m_Window.GetAspectResolution()) {
	m_Window.SetEventCallback(std::bind(&Application::HandleEvent, this, std::placeholders::_1));
}

void Application::Run() {
	Origo::Logger::Init();
	Origo::Input::SetContext(Origo::MakeRef<Origo::ScreenWindow>(m_Window));

	Awake();

	while (m_Running) {
		Time::Duration dt { Time::GetNow() - m_LastTimeStamp };

		InternalUpdate();
		Update(dt.count());

		Renderer::Flush(m_Scene.GetCamera());

		if (m_Window.ShouldClose()) {
			m_Running = false;
		}

		m_LastTimeStamp = Time::GetNow();

		m_Window.OnUpdate();
	}
}
}
