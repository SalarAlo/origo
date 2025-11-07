#pragma once

#include "origo/events/Event.h"

namespace Origo {

class Layer {
public:
	Layer(std::string_view name = "Layer");
	virtual ~Layer();

	virtual void OnAttach() { };
	virtual void OnDetach() { };
	virtual void OnUpdate(double dt) { };
	virtual void OnEvent(Event& e) { };

	std::string GetName() const { return m_DebugName; }

private:
	std::string m_DebugName;
};

}
