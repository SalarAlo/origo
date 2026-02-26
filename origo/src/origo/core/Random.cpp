#include <random>

#include "origo/core/Random.h"

static std::mt19937_64 rng;

namespace Origo::Random {

void seed(int seed) {
	rng.seed(seed);
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

}
