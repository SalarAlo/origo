#pragma once

#include "origo/components/Component.h"

namespace Origo {
class EditorHiddenComponent : public Origo::Component {
public:
	std::string get_component_name() const override { return "InspectorHidden"; }

private:
};

}
