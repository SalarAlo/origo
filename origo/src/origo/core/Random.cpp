#include <random>

#include "origo/core/Random.h"

static std::mt19937_64 rng;

namespace Origo::Random {

void Seed(int seed) {
	rng.seed(seed);
}

float Range(float min, float max) {
	min = std::min(min, max);
	max = std::max(min, max);

	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

int Range(int min, int max) {
	min = std::min(min, max);
	max = std::max(min, max);

	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

Vec3 RandomUnitVector() {
	float u = Range(-1.0f, 1.0f);
	float theta = Range(0.0f, 2.0f * 3.14159265358979323846f);

	float sqrtTerm = std::sqrt(1.0f - u * u);

	auto out = Vec3(
	    sqrtTerm * std::cos(theta),
	    sqrtTerm * std::sin(theta),
	    u);
	out = glm::normalize(out);
	return out;
}

}
