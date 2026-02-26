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
		Backend.Write("type", magic_enum::enum_name(ParticleEmissionShapeKind::Point));
	}

	void operator()(ConeEmissionShape shape) {
		Backend.Write("type", magic_enum::enum_name(ParticleEmissionShapeKind::Cone));
		Backend.Write("angle", shape.Angle);
		Backend.Write("length", shape.Length);
	}

	void operator()(SphereEmissionShape shape) {
		Backend.Write("type", magic_enum::enum_name(ParticleEmissionShapeKind::Sphere));
		Backend.Write("radius", shape.Radius);
	}

	void operator()(BoxEmissionShape shape) {
		Backend.Write("type", magic_enum::enum_name(ParticleEmissionShapeKind::Box));
		Backend.Write("x", shape.Position.x);
		Backend.Write("y", shape.Position.y);
		Backend.Write("z", shape.Position.z);
	}

	ParticleEmissionShape operator()(ISerializer& backend) {
		if (std::string type_str {}; backend.TryRead("type", type_str)) {
			auto type { magic_enum::enum_cast<ParticleEmissionShapeKind>(type_str) };

			if (!type.has_value())
				return ParticleEmissionShapeFactory::CreateDefault(ParticleEmissionShapeKind::Point);

			switch (*type) {
			case ParticleEmissionShapeKind::Point:
				return ParticleEmissionShapeFactory::CreateDefault(ParticleEmissionShapeKind::Point);
			case ParticleEmissionShapeKind::Sphere: {
				auto sphere { ParticleEmissionShapeFactory::CreateDefault(ParticleEmissionShapeKind::Sphere) };
				backend.TryRead("radius", std::get<SphereEmissionShape>(sphere).Radius);
				return sphere;
			}
			case ParticleEmissionShapeKind::Cone: {
				auto cone { ParticleEmissionShapeFactory::CreateDefault(ParticleEmissionShapeKind::Cone) };
				backend.TryRead("angle", std::get<ConeEmissionShape>(cone).Angle);
				backend.TryRead("length", std::get<ConeEmissionShape>(cone).Length);
				return cone;
			}
			case ParticleEmissionShapeKind::Box: {
				auto box { ParticleEmissionShapeFactory::CreateDefault(ParticleEmissionShapeKind::Box) };
				backend.TryRead("x", std::get<BoxEmissionShape>(box).Position.x);
				backend.TryRead("y", std::get<BoxEmissionShape>(box).Position.y);
				backend.TryRead("z", std::get<BoxEmissionShape>(box).Position.z);
				return box;
			}
			}
		}

		throw std::logic_error("unimplemented emission shape kind");
	}

	ISerializer& Backend;
};

}
