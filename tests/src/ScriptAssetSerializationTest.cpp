#include <doctest/doctest.h>

#include <filesystem>
#include <string>

#include "origo/assets/Script.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/serialization/JsonSerializer.h"

namespace {
std::filesystem::path make_temp_path(std::string_view tag) {
	auto base = std::filesystem::temp_directory_path();
	const auto unique = std::to_string(
	    std::filesystem::file_time_type::clock::now().time_since_epoch().count());
	return base / ("origo_" + std::string(tag) + "_" + unique + ".json");
}
}

TEST_CASE("Script asset serializes and deserializes via AssetSerializationSystem") {
	const std::filesystem::path json_path = make_temp_path("script_asset_ser");

	Origo::AssetSerializationSystem::cleanup();
	Origo::AssetSerializationSystem::register_all_asset_serializers();

	auto* serializer = Origo::AssetSerializationSystem::get(Origo::AssetType::Script);
	REQUIRE(serializer != nullptr);

	const Origo::UUID expected_id = Origo::UUID::from_hash("script-asset-test-id");
	Origo::Script original("assets/scripts/player.lua", expected_id);

	Origo::JsonSerializer writer(json_path);
	serializer->serialize(&original, writer);
	writer.save_to_file();

	Origo::Script restored;

	Origo::JsonSerializer reader(json_path);
	reader.load_file();
	serializer->deserialize(reader, restored);

	CHECK(restored.get_id() == expected_id);
	CHECK(restored.get_path().string() == std::string("assets/scripts/player.lua"));

	Origo::AssetSerializationSystem::cleanup();
	std::error_code ec;
	std::filesystem::remove(json_path, ec);
}
