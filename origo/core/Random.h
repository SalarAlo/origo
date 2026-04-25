#pragma once

#include "origo/serialization/ISerializer.h"

#include <FastNoiseLite.h>

namespace Origo::Random {

void seed(int seed);

float range(float min, float max);
float float01();
int range(int min, int max);

Vec3 random_unit_vector();

}

namespace Origo::Noise {
enum class Type {
	Perlin
};

enum class FractalType {
	None,
	FBm
};

struct Settings {
	Type type = Type::Perlin;
	int seed = 1337;
	float frequency = 0.04f;

	FractalType fractal = FractalType::FBm;
	int octaves = 3;
	float lacunarity = 5.0f;
	float gain = 0.1f;
	float weighted_strength = 1.0f;

	void serialize(ISerializer& serializer) const;
	void deserialize(ISerializer& serializer);
};

class Generator {
public:
	Generator();
	Generator(const Settings& settings);

	void set_settings(const Settings& settings);

	float sample_2d(float x, float y) const;
	float sample_3d(float x, float y, float z) const;

private:
	void apply_settings();

private:
	Settings m_settings;
	FastNoiseLite m_impl;
};

}
