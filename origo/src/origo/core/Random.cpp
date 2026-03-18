#include <cmath>

#include <algorithm>
#include <random>

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

#include "origo/core/Random.h"

static std::mt19937_64 rng;
static int s_noise_seed = 1337;

namespace Origo::Random {

void seed(int seed) {
	rng.seed(seed);
	s_noise_seed = seed;
}

float range(float min, float max) {
	min = std::min(min, max);
	max = std::max(min, max);

	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

float float01() {
	return range(0.0f, 1.0f);
}

int range(int min, int max) {
	min = std::min(min, max);
	max = std::max(min, max);

	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

Vec3 random_unit_vector() {
	float u = range(-1.0f, 1.0f);
	float theta = range(0.0f, 2.0f * 3.14159265358979323846f);

	float sqrt_term = std::sqrt(1.0f - u * u);

	auto out = Vec3(
	    sqrt_term * std::cos(theta),
	    sqrt_term * std::sin(theta),
	    u);
	out = glm::normalize(out);
	return out;
}

float perlin(float x) {
	return perlin(x, 0.0f, 0.0f);
}

float perlin(float x, float y) {
	return perlin(x, y, 0.0f);
}

float perlin(float x, float y, float z) {
	return 0.5f * (stb_perlin_noise3_seed(x, y, z, 0, 0, 0, s_noise_seed) + 1.0f);
}

float perlin_octaves(float x, float y, const PerlinSettings& s) {
	const int octaves = std::max(1, s.Octaves);
	const float base_frequency = std::max(0.0001f, s.Frequency);
	const float lacunarity = std::max(0.0001f, s.Lacunarity);
	const float persistence = std::max(0.0f, s.Persistence);

	float total = 0.0f;
	float frequency = base_frequency;
	float amplitude = 1.0f;
	float max_value = 0.0f;

	x *= s.Scale;
	y *= s.Scale;

	for (int i = 0; i < octaves; ++i) {
		const float sample = stb_perlin_noise3_seed(
		    x * frequency,
		    y * frequency,
		    0.0f,
		    0,
		    0,
		    0,
		    s.Seed + i);
		total += sample * amplitude;

		max_value += amplitude;

		amplitude *= persistence;
		frequency *= lacunarity;
	}

	if (max_value <= 0.0f)
		return 0.0f;

	return (total / max_value) * s.Amplitude;
}

}
