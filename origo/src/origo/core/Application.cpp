#include "origo/core/Application.h"

namespace Origo {
Application::Application(const ApplicationSettings& settings)
    : m_Settings(settings)
    , m_Window(settings.WindowSettings)
    , m_Running(true) {
}

void Application::Run() {
	OnInit();

	while (m_Running) {
		OnUpdate(0);

		m_Window.SwapBuffers();
		if (m_Window.ShouldClose()) {
			m_Running = false;
		}
	}
}
}
