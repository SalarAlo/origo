#pragma once

#include "origo/core/LayerStack.h"
#include "origo/core/Time.h"
#include "origo/events/Event.h"
#include "origo/core/ScreenWindow.h"

namespace Origo {
// TODO: Split WorkingDir and AssetDir
struct ApplicationSettings {
	std::string WorkingDirectory {};
	ScreenWindowSettings WindowSettings {};
};

class Application {
public:
	Application(const ApplicationSettings& settings = {});
	virtual void Run();

	virtual void OnAwake() { }
	virtual void OnShutdown() { }
	void OnEvent(Event& event);

	void PushLayer(Layer*);
	void PushOverlay(Layer*);

private:
	void InternalAwake();
	void InternalUpdate(double dt);
	void InternalShutdown();

protected:
	ScreenWindow m_Window;
	ApplicationSettings m_Settings {};
	LayerStack m_LayerStack {};

	bool m_Running {};
	Time::TimePoint m_LastTimeStamp {};
};
}
