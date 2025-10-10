#include "origo/core/Application.h"
#include "origo/core/Time.h"
#include <functional>

namespace Origo {

Application::Application(const ApplicationSettings& settings)
    : m_Settings(settings)
    , m_Window(settings.WindowSettings)
    , m_Running(true)
    , m_LastTimeStamp(Time::GetNow()) {
	m_Window.SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
}

void Application::Run() {
	OnInit();

	while (m_Running) {
		Time::Duration dt { Time::GetNow() - m_LastTimeStamp };

		OnUpdate(dt.count());

		OnRender();

		if (m_Window.ShouldClose()) {
			m_Running = false;
		}

		m_LastTimeStamp = Time::GetNow();

		m_Window.OnUpdate();
	}
}
}
