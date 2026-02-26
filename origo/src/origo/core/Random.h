#pragma once

namespace Origo::Random {
void seed(int seed);

float range(float min, float max);
float float01();
int range(int min, int max);

Vec3 random_unit_vector();

}
