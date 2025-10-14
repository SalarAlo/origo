#pragma once

#include "origo/events/Event.h"
#include "origo/core/ScreenWindow.h"
#include "origo/core/Time.h"
#include "origo/scene/Scene.h"

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
	virtual void OnUpdate(double deltaTime) { }
	virtual void OnEvent(Event& event) { }

protected:
	ScreenWindow m_Window;
	Scene m_Scene;

private:
	bool m_Running {};
	Time::TimePoint m_LastTimeStamp {};
	ApplicationSettings m_Settings {};
};
}
