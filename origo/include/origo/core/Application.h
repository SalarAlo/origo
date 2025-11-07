#pragma once

#include "origo/core/LayerStack.h"
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

	virtual void OnAwake() { }
	virtual void OnShutdown() { }
	virtual void OnUpdate(double deltaTime) { }
	virtual void OnEvent(Event& event);

	void PushLayer(Layer*);
	void PushOverlay(Layer*);

private:
	void InternalUpdate(double dt);

protected:
	ScreenWindow m_Window;
	Scene m_Scene;

private:
	bool m_Running {};
	Time::TimePoint m_LastTimeStamp {};
	ApplicationSettings m_Settings {};
	LayerStack m_LayerStack {};
};
}
