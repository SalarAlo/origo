#include <doctest/doctest.h>

#include <filesystem>
#include <string>

#include "origo/assets/SceneAsset.h"

#include "origo/assets/serialization/SceneAssetSerializer.h"

#include "origo/serialization/JsonSerializer.h"

namespace {
std::filesystem::path make_temp_path(std::string_view tag) {
	auto base = std::filesystem::temp_directory_path();
	const auto unique = std::to_string(std::filesystem::file_time_type::clock::now().time_since_epoch().count());
	return base / ("origo_" + std::string(tag) + "_" + unique + ".json");
}
}

TEST_CASE("SceneAssetSerializer roundtrip preserves scene path") {
	const std::filesystem::path json_path = make_temp_path("scene_asset_ser");

	Origo::SceneAssetSerializer serializer;
	Origo::SceneAsset original("assets/scenes/level_01.scene");

	Origo::JsonSerializer writer(json_path);
	serializer.serialize(&original, writer);
	writer.save_to_file();

	Origo::SceneAsset restored;

	Origo::JsonSerializer reader(json_path);
	reader.load_file();
	serializer.deserialize(reader, restored);

	REQUIRE(restored.get_path().has_value());
	CHECK(restored.get_path()->string() == std::string("assets/scenes/level_01.scene"));

	std::error_code ec;
	std::filesystem::remove(json_path, ec);
}
