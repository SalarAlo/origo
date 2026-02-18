#pragma once

namespace Origo::Random {
void Seed(int seed);

float Range(float min, float max);
int Range(int min, int max);

Vec3 RandomUnitVector();

}
