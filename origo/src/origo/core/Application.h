#pragma once

#include "origo/core/LayerSystem.h"
#include "origo/core/ScreenWindow.h"
#include "origo/core/Time.h"

#include "origo/events/Event.h"

namespace Origo {
struct ApplicationSettings {
	std::string WorkingDirectory {};
	ScreenWindowSettings WindowSettings {};
};

class Application {
public:
	Application(const ApplicationSettings& settings = {});
	virtual void run();

	virtual void on_awake() { }
	virtual void on_end_frame(float dt) { };
	virtual void on_shutdown() { }
	void on_event(Event& event);

	void push_layer(Layer*, size_t, bool frozen = false);
	void pop_layer(size_t);

private:
	void internal_awake();
	void internal_update(double dt);
	void internal_shutdown();

protected:
	ScreenWindow m_window;
	ApplicationSettings m_settings {};
	LayerSystem m_layer_system {};

	bool m_running {};
	Time::TimePoint m_last_time_stamp {};
};
}
