#include <cmath>

#include <algorithm>
#include <random>

#include <magic_enum/magic_enum.hpp>

#include "origo/core/Random.h"

#include "FastNoiseLite.h"

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

}

namespace Origo::Noise {

void Settings::serialize(ISerializer& serializer) const {
	serializer.write("type", std::string_view(magic_enum::enum_name(type)));
	serializer.write("seed", seed);
	serializer.write("frequency", frequency);

	serializer.write("fractal", std::string_view(magic_enum::enum_name(fractal)));
	serializer.write("octaves", octaves);
	serializer.write("lacunarity", lacunarity);
	serializer.write("gain", gain);
	serializer.write("weighted_strength", weighted_strength);
}

void Settings::deserialize(ISerializer& serializer) {
	if (std::string type_name; serializer.try_read("type", type_name)) {
		if (auto parsed = magic_enum::enum_cast<Type>(type_name))
			type = *parsed;
	}

	serializer.try_read("seed", seed);
	serializer.try_read("frequency", frequency);

	if (std::string fractal_name; serializer.try_read("fractal", fractal_name)) {
		if (auto parsed = magic_enum::enum_cast<FractalType>(fractal_name))
			fractal = *parsed;
	}

	serializer.try_read("octaves", octaves);
	serializer.try_read("lacunarity", lacunarity);
	serializer.try_read("gain", gain);
	serializer.try_read("weighted_strength", weighted_strength);
}

Generator::Generator()
    : m_impl() {
	apply_settings();
}

Generator::Generator(const Settings& settings)
    : m_settings(settings)
    , m_impl() {
	apply_settings();
}

void Generator::set_settings(const Settings& settings) {
	m_settings = settings;
	apply_settings();
}

void Generator::apply_settings() {
	auto& n = m_impl;

	n.SetSeed(m_settings.seed);
	n.SetFrequency(m_settings.frequency);

	switch (m_settings.type) {
	case Type::Perlin:
		n.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		break;
	}

	switch (m_settings.fractal) {
	case FractalType::None:
		n.SetFractalType(FastNoiseLite::FractalType_None);
		break;
	case FractalType::FBm:
		n.SetFractalType(FastNoiseLite::FractalType_FBm);
		break;
	}

	n.SetFractalOctaves(m_settings.octaves);
	n.SetFractalLacunarity(m_settings.lacunarity);
	n.SetFractalGain(m_settings.gain);
	n.SetFractalWeightedStrength(m_settings.weighted_strength);
}

float Generator::sample_2d(float x, float y) const {
	return m_impl.GetNoise(x, y);
}

float Generator::sample_3d(float x, float y, float z) const {
	return m_impl.GetNoise(x, y, z);
}

}
