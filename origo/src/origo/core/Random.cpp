#include <cmath>

#include <algorithm>
#include <random>

#include "origo/core/Random.h"

static std::mt19937_64 rng;

static int perm[512];

namespace {

float fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float self_lerp(float a, float b, float t) {
	return a + t * (b - a);
}

float grad(int hash, float x, float y, float z) {
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);

	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

void build_permutation() {
	int p[256];

	for (int i = 0; i < 256; ++i)
		p[i] = i;

	// Shuffle using your RNG
	for (int i = 255; i > 0; --i) {
		int j = std::uniform_int_distribution<int>(0, i)(rng);
		std::swap(p[i], p[j]);
	}

	for (int i = 0; i < 256; ++i) {
		perm[i] = p[i];
		perm[i + 256] = p[i];
	}
}

}

namespace Origo::Random {

void seed(int seed) {
	rng.seed(seed);
	build_permutation(); // IMPORTANT: tie noise to seed
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
	int floored_x = (int)std::floor(x) & 255;
	int floored_y = (int)std::floor(y) & 255;
	int floored_z = (int)std::floor(z) & 255;

	x -= std::floor(x);
	y -= std::floor(y);
	z -= std::floor(z);

	float u = fade(x);
	float v = fade(y);
	float w = fade(z);

	int a = perm[floored_x] + floored_y;
	int aa = perm[a] + floored_z;
	int ab = perm[a + 1] + floored_z;
	int b = perm[floored_x + 1] + floored_y;
	int ba = perm[b] + floored_z;
	int bb = perm[b + 1] + floored_z;

	float res = self_lerp(
	    self_lerp(
	        self_lerp(grad(perm[aa], x, y, z),
	            grad(perm[ba], x - 1, y, z), u),
	        self_lerp(grad(perm[ab], x, y - 1, z),
	            grad(perm[bb], x - 1, y - 1, z), u),
	        v),
	    self_lerp(
	        self_lerp(grad(perm[aa + 1], x, y, z - 1),
	            grad(perm[ba + 1], x - 1, y, z - 1), u),
	        self_lerp(grad(perm[ab + 1], x, y - 1, z - 1),
	            grad(perm[bb + 1], x - 1, y - 1, z - 1), u),
	        v),
	    w);

	return (res + 1.0f) * 0.5f;
}

float perlin_octaves(float x, float y, const PerlinSettings& s) {
	float total = 0.0f;
	float frequency = s.Frequency;
	float amplitude = s.Amplitude;
	float max_value = 0.0f;

	x *= s.Scale;
	y *= s.Scale;

	for (int i = 0; i < s.Octaves; ++i) {
		total += perlin(x * frequency, y * frequency) * amplitude;

		max_value += amplitude;

		amplitude *= s.Persistence;
		frequency *= s.Lacunarity;
	}

	return total / max_value;
}

}
