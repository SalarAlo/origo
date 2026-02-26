
#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ConeEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

#include "ui/ComponentUI.h"

namespace OrigoEditor {
struct DrawEmissionShapeGUIControls {
	void operator()(Origo::PointEmissionShape& p) {
	}

	void operator()(Origo::SphereEmissionShape& sp) {
		ComponentUI::draw_float_control("radius", sp.Radius);
	}

	void operator()(Origo::BoxEmissionShape& b) {
		ComponentUI::draw_vec3_control("size", b.Size);
	}

	void operator()(Origo::ConeEmissionShape& c) {
		ComponentUI::draw_float_control("length", c.Length);
		ComponentUI::draw_float_control("angle", c.Angle);
	}
};

}
