#include "TerrainComponentSerializer.h"

#include "origo/components/TerrainComponent.h"

namespace Origo {
void TerrainComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto terrain { dynamic_cast<TerrainComponent*>(comp) };
	s.write("amplitude", terrain->Amplitude);
	s.write("octaves", terrain->Octaves);
	s.write("frequency", terrain->Frequency);
	s.write("scale", terrain->Scale);
	s.write("size", terrain->Size);
}

void TerrainComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto terrain { dynamic_cast<TerrainComponent*>(comp) };

	s.try_read("amplitude", terrain->Amplitude);
	s.try_read("octaves", terrain->Octaves);
	s.try_read("frequency", terrain->Frequency);
	s.try_read("scale", terrain->Scale);
	s.try_read("size", terrain->Size);
}
}
