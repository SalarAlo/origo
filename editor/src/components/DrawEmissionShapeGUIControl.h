
#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

#include "ui/ComponentUI.h"

namespace OrigoEditor {
struct DrawEmissionShapeGUIControls {
	void operator()(Origo::PointEmissionShape& p) {
	}

	void operator()(Origo::SphereEmissionShape& sp) {
		ComponentUI::DrawFloatControl("radius", sp.Radius);
	}

	void operator()(Origo::BoxEmissionShape& b) {
		ComponentUI::DrawVec3Control("size", b.Size);
	}
};

}
