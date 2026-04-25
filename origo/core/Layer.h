#pragma once

#include "origo/events/Event.h"

namespace Origo {

class Layer {
public:
	Layer(std::string_view name = "Layer");
	virtual ~Layer();

	virtual void on_attach() { };
	virtual void on_detach() { };
	virtual void on_update(double dt) { };
	virtual void on_event(Event& e) { };

	std::string get_name() const { return m_debug_name; }

private:
	std::string m_debug_name;
};

}
