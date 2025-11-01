#pragma once

#include "origo/events/Event.h"
#include "origo/core/ScreenWindow.h"
#include "origo/core/Time.h"
#include "origo/imgui/ImGuiLayer.h"
#include "origo/renderer/Renderer.h"
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
	virtual void OnRender() {
		Renderer::Flush(m_Scene.GetMainCamera());
	}

	virtual void OnShutdown() { }
	virtual void OnUpdate(double deltaTime) { }
	virtual void OnImGuiRender() { }
	virtual void OnHandleEvent(Event& event) { }

private:
	void InternalUpdate();

protected:
	ScreenWindow m_Window;
	Scene m_Scene;

private:
	bool m_Running {};
	ImGuiLayer m_ImGuiLayer {};
	Time::TimePoint m_LastTimeStamp {};
	ApplicationSettings m_Settings {};
};
}
