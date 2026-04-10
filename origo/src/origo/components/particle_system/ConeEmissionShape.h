#pragma once

namespace Origo {

struct ConeEmissionShape {
	Vec3 Position { 0.0f };
	Vec3 Direction { 0.0f, 1.0f, 0.0f };
	float Angle { 40.0f };
	float Length { 1.0f };
};

}
