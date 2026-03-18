#pragma once

namespace Origo::Random {

struct PerlinSettings {
	float Frequency = 0.5f;
	float Amplitude = 1.0f;
	int Octaves = 3;
	float Scale = 0.02f;
	float Persistence = 0.5f;
	float Lacunarity = 2.0f;
	int Seed = 1337;
};

void seed(int seed);

float range(float min, float max);
float float01();
int range(int min, int max);

Vec3 random_unit_vector();

float perlin(float x);
float perlin(float x, float y);
float perlin(float x, float y, float z);

float perlin_octaves(float x, float y, const PerlinSettings& s);

}
