#pragma once

#include "origo/events/Event.h"
#include "origo/ScreenWindow.h"
#include "origo/core/Time.h"

namespace Origo {
struct ApplicationSettings {
	std::string WorkingDirectory {};
	ScreenWindowSettings WindowSettings {};
};

class Application {
public:
	Application(const ApplicationSettings& settings = {});
	void Run();

	static Application& Get();

	virtual void OnInit() { }
	virtual void OnShutdown() { }
	virtual void OnUpdate(float deltaTime) { }
	virtual void OnRender() { }
	virtual void OnEvent(Event& event) { }

private:
	bool m_Running {};
	Time::TimePoint m_LastTimeStamp {};
	ApplicationSettings m_Settings {};
	ScreenWindow m_Window;
};
}
