#include "TerrainComponentSerializer.h"

#include <magic_enum/magic_enum.hpp>

#include "origo/components/TerrainComponent.h"

namespace Origo {
namespace {
OptionalAssetHandle read_optional_asset(ISerializer& serializer, std::string_view key) {
	std::string id {};
	if (!serializer.try_read(key, id) || id.empty() || id == "0")
		return std::nullopt;

	return AssetManager::get_instance().get_handle_by_uuid(UUID::from_string(id));
}
}

void TerrainComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto* terrain = static_cast<TerrainComponent*>(comp);

	terrain->noise_settings.serialize(s);
	s.begin_object("shape_settings");
	terrain->shape_settings.serialize(s);
	s.end_object();
	s.begin_object("surface_settings");
	terrain->surface_settings.serialize(s);
	s.end_object();
	s.begin_object("water_settings");
	terrain->water_settings.serialize(s);
	s.end_object();
	s.write("style", std::string_view(magic_enum::enum_name(terrain->style)));
	s.write("use_texture_layers", terrain->use_texture_layers);
	s.write("ground_albedo", terrain->ground_albedo);
	s.write("ground_normal", terrain->ground_normal);
	s.write("ground_packed", terrain->ground_packed);
	s.write("rock_albedo", terrain->rock_albedo);
	s.write("rock_normal", terrain->rock_normal);
	s.write("rock_packed", terrain->rock_packed);
	s.write("sand_albedo", terrain->sand_albedo);
	s.write("sand_normal", terrain->sand_normal);
	s.write("sand_packed", terrain->sand_packed);
	s.write("snow_albedo", terrain->snow_albedo);
	s.write("snow_normal", terrain->snow_normal);
	s.write("snow_packed", terrain->snow_packed);
	s.write("micro_normal", terrain->micro_normal);
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
	s.begin_object("shape_settings");
	terrain->shape_settings.deserialize(s);
	s.end_object();
	s.begin_object("surface_settings");
	terrain->surface_settings.deserialize(s);
	s.end_object();
	s.begin_object("water_settings");
	terrain->water_settings.deserialize(s);
	s.end_object();
	if (std::string style_name; s.try_read("style", style_name)) {
		if (auto style = magic_enum::enum_cast<TerrainStyle>(style_name))
			terrain->style = *style;
	}
	s.try_read("use_texture_layers", terrain->use_texture_layers);
	terrain->ground_albedo = read_optional_asset(s, "ground_albedo");
	terrain->ground_normal = read_optional_asset(s, "ground_normal");
	terrain->ground_packed = read_optional_asset(s, "ground_packed");
	terrain->rock_albedo = read_optional_asset(s, "rock_albedo");
	terrain->rock_normal = read_optional_asset(s, "rock_normal");
	terrain->rock_packed = read_optional_asset(s, "rock_packed");
	terrain->sand_albedo = read_optional_asset(s, "sand_albedo");
	terrain->sand_normal = read_optional_asset(s, "sand_normal");
	terrain->sand_packed = read_optional_asset(s, "sand_packed");
	terrain->snow_albedo = read_optional_asset(s, "snow_albedo");
	terrain->snow_normal = read_optional_asset(s, "snow_normal");
	terrain->snow_packed = read_optional_asset(s, "snow_packed");
	terrain->micro_normal = read_optional_asset(s, "micro_normal");
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
