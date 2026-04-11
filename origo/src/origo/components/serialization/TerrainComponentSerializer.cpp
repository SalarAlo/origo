#include "TerrainComponentSerializer.h"

#include "origo/components/TerrainComponent.h"

namespace Origo {

void TerrainComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto* terrain = static_cast<TerrainComponent*>(comp);

	terrain->noise_settings.serialize(s);
	s.write("size_x", terrain->size_x);
	s.write("size_z", terrain->size_z);
	s.write("scale", terrain->scale);
	s.write("cell_size", terrain->cell_size);
	s.write("height", terrain->height);
	s.write("contrast", terrain->contrast);

	s.begin_object("erosion_settings");
	terrain->erosion_settings.serialize(s);
	s.end_object();
}

void TerrainComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto* terrain = static_cast<TerrainComponent*>(comp);

	terrain->noise_settings.deserialize(s);
	s.try_read("size_x", terrain->size_x);
	s.try_read("size_z", terrain->size_z);
	s.try_read("scale", terrain->scale);
	s.try_read("cell_size", terrain->cell_size);
	s.try_read("height", terrain->height);
	s.try_read("contrast", terrain->contrast);

	s.begin_object("erosion_settings");
	terrain->erosion_settings.deserialize(s);
	s.end_object();
}

}
