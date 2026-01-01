#pragma once

#include "origo/core/LayerSystem.h"
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

	void PushLayer(Layer*, size_t);
	void PopLayer(size_t);

private:
	void InternalAwake();
	void InternalUpdate(double dt);
	void InternalShutdown();

protected:
	ScreenWindow m_Window;
	ApplicationSettings m_Settings {};
	LayerSystem m_LayerSystem {};

	bool m_Running {};
	Time::TimePoint m_LastTimeStamp {};
};
}
