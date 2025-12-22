#pragma once

#include "origo/core/LayerStack.h"
#include "origo/events/Event.h"
#include "origo/core/ScreenWindow.h"
#include "origo/core/Time.h"
#include "origo/renderer/FrameBuffer.h"
#include "origo/renderer/RenderContext.h"
#include "origo/scene/Scene.h"

namespace Origo {
// TODO: Split WorkingDir and AssetDir
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
	virtual void OnEvent(Event& event);

	void PushLayer(Layer*);
	void PushOverlay(Layer*);

private:
	void InternalUpdate(double dt);
	void OnRender();
	void InternalShutdown();
	void InternalAwake();

protected:
	ScreenWindow m_Window;
	Scene m_Scene;
	RenderContext m_RenderContext;

private:
	bool m_Running {};
	Time::TimePoint m_LastTimeStamp {};
	ApplicationSettings m_Settings {};
	LayerStack m_LayerStack {};
};
}
