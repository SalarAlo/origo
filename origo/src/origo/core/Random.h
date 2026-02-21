#pragma once

namespace Origo::Random {
void Seed(int seed);

float Range(float min, float max);
float Float01();
int Range(int min, int max);

Vec3 RandomUnitVector();

}
