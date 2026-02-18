#pragma once

#include "origo/components/Component.h"

namespace Origo {
class EditorHiddenComponent : public Origo::Component {
public:
	std::string GetComponentName() const override { return "InspectorHidden"; }

private:
};

}
