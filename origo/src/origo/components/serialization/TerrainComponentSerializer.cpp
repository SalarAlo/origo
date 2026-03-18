#include "TerrainComponentSerializer.h"

#include "origo/components/TerrainComponent.h"

namespace Origo {
void TerrainComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto terrain { dynamic_cast<TerrainComponent*>(comp) };
	s.write("amplitude", terrain->Perlin.Amplitude);
	s.write("octaves", terrain->Perlin.Octaves);
	s.write("frequency", terrain->Perlin.Frequency);
	s.write("scale", terrain->Perlin.Scale);
	s.write("detail", terrain->Detail);
	s.write("height", terrain->Height);
	s.write("persistence", terrain->Perlin.Persistence);
	s.write("lacunarity", terrain->Perlin.Lacunarity);
	s.write("seed", terrain->Perlin.Seed);
	s.write("size", terrain->Size);
}

void TerrainComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto terrain { dynamic_cast<TerrainComponent*>(comp) };

	s.try_read("amplitude", terrain->Perlin.Amplitude);
	s.try_read("octaves", terrain->Perlin.Octaves);
	s.try_read("frequency", terrain->Perlin.Frequency);
	s.try_read("scale", terrain->Perlin.Scale);
	s.try_read("detail", terrain->Detail);
	s.try_read("height", terrain->Height);
	s.try_read("persistence", terrain->Perlin.Persistence);
	s.try_read("lacunarity", terrain->Perlin.Lacunarity);
	s.try_read("seed", terrain->Perlin.Seed);
	s.try_read("size", terrain->Size);

	terrain->RebuildNecessary = true;
}
}
