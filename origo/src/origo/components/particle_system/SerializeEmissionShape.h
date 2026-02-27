#pragma once

#include <stdexcept>

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ConeEmissionShape.h"
#include "origo/components/particle_system/ParticleEmissionShape.h"
#include "origo/components/particle_system/ParticleEmissionShapeFactory.h"
#include "origo/components/particle_system/ParticleEmissionShapeKind.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {
struct SerializeEmissionShape {
	SerializeEmissionShape(ISerializer& backend)
	    : Backend(backend) { };

	void operator()(PointEmissionShape shape) {
		Backend.write("type", magic_enum::enum_name(ParticleEmissionShapeKind::Point));
	}

	void operator()(ConeEmissionShape shape) {
		Backend.write("type", magic_enum::enum_name(ParticleEmissionShapeKind::Cone));
		Backend.write("angle", shape.Angle);
		Backend.write("length", shape.Length);
	}

	void operator()(SphereEmissionShape shape) {
		Backend.write("type", magic_enum::enum_name(ParticleEmissionShapeKind::Sphere));
		Backend.write("radius", shape.Radius);
	}

	void operator()(BoxEmissionShape shape) {
		Backend.write("type", magic_enum::enum_name(ParticleEmissionShapeKind::Box));
		Backend.write("x", shape.Position.x);
		Backend.write("y", shape.Position.y);
		Backend.write("z", shape.Position.z);
	}

	ParticleEmissionShape operator()() {
		if (std::string type_str {}; Backend.try_read("type", type_str)) {
			auto type { magic_enum::enum_cast<ParticleEmissionShapeKind>(type_str) };

			if (!type.has_value())
				return ParticleEmissionShapeFactory::create_default(ParticleEmissionShapeKind::Point);

			switch (*type) {
			case ParticleEmissionShapeKind::Point:
				return ParticleEmissionShapeFactory::create_default(ParticleEmissionShapeKind::Point);
			case ParticleEmissionShapeKind::Sphere: {
				auto sphere { ParticleEmissionShapeFactory::create_default(ParticleEmissionShapeKind::Sphere) };
				Backend.try_read("radius", std::get<SphereEmissionShape>(sphere).Radius);
				return sphere;
			}
			case ParticleEmissionShapeKind::Cone: {
				auto cone { ParticleEmissionShapeFactory::create_default(ParticleEmissionShapeKind::Cone) };
				Backend.try_read("angle", std::get<ConeEmissionShape>(cone).Angle);
				Backend.try_read("length", std::get<ConeEmissionShape>(cone).Length);
				return cone;
			}
			case ParticleEmissionShapeKind::Box: {
				auto box { ParticleEmissionShapeFactory::create_default(ParticleEmissionShapeKind::Box) };
				Backend.try_read("x", std::get<BoxEmissionShape>(box).Position.x);
				Backend.try_read("y", std::get<BoxEmissionShape>(box).Position.y);
				Backend.try_read("z", std::get<BoxEmissionShape>(box).Position.z);
				return box;
			}
			}
		}

		throw std::logic_error("unimplemented emission shape kind");
	}

	ISerializer& Backend;
};

}
