#include "TerrainComponentSerializer.h"

#include "origo/components/TerrainComponent.h"

namespace Origo {

void TerrainComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto* terrain = static_cast<TerrainComponent*>(comp);

	terrain->settings.serialize(s);
	s.write("scale", terrain->scale);
	s.write("contrast", terrain->contrast);
}

void TerrainComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto* terrain = static_cast<TerrainComponent*>(comp);

	terrain->settings.deserialize(s);
	s.try_read("scale", terrain->scale);
	s.try_read("contrast", terrain->contrast);
}

}
