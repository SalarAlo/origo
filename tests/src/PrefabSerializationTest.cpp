#include <doctest/doctest.h>

#include <filesystem>
#include <string>

#include "origo/assets/Prefab.h"
#include "origo/assets/serialization/PrefabSerializer.h"
#include "origo/assets/serialization/SceneEntitySerializationManager.h"
#include "origo/components/Name.h"
#include "origo/scene/Scene.h"
#include "origo/serialization/JsonSerializer.h"

namespace {
std::filesystem::path make_temp_path(std::string_view tag) {
	auto base = std::filesystem::temp_directory_path();
	const auto unique = std::to_string(
	    std::filesystem::file_time_type::clock::now().time_since_epoch().count());
	return base / ("origo_" + std::string(tag) + "_" + unique + ".prefab");
}
}

TEST_CASE("PrefabSerializer preserves authored prefab payload loaded from disk") {
	const std::filesystem::path prefab_path = make_temp_path("prefab_ser");

	Origo::JsonSerializer writer(prefab_path);
	writer.write("rid", std::string_view("42"));
	writer.begin_array("native_components");
	writer.begin_array_object();
	writer.write("type", std::string_view("Name"));
	writer.begin_object("data");
	writer.write("name", std::string_view("PrefabCube"));
	writer.end_object();
	writer.end_array_object();
	writer.end_array();
	writer.begin_array("script_components");
	writer.end_array();
	writer.save_to_file();

	Origo::JsonSerializer reader(prefab_path);
	reader.load_file();

	std::string reader_rid;
	REQUIRE(reader.try_read("rid", reader_rid));
	CHECK(reader_rid == "42");

	Origo::JsonSerializer copied;
	reader.write_to(copied);

	std::string copied_rid;
	REQUIRE(copied.try_read("rid", copied_rid));
	CHECK(copied_rid == "42");

	Origo::Prefab prefab;
	Origo::PrefabSerializer serializer;
	serializer.deserialize(reader, prefab);

	std::string rid;
	REQUIRE(prefab.backend.try_read("rid", rid));
	CHECK(rid == "42");

	prefab.backend.begin_array("native_components");
	REQUIRE(prefab.backend.try_begin_array_object_read());

	std::string type;
	REQUIRE(prefab.backend.try_read("type", type));
	CHECK(type == "Name");

	prefab.backend.begin_object("data");
	std::string name;
	REQUIRE(prefab.backend.try_read("name", name));
	CHECK(name == "PrefabCube");
	prefab.backend.end_object();

	prefab.backend.end_array_object();
	prefab.backend.end_array();

	std::error_code ec;
	std::filesystem::remove(prefab_path, ec);
}

TEST_CASE("Prefab payload can be instantiated from a fresh reader even if the prefab serializer cursor is stale") {
	const std::filesystem::path prefab_path = make_temp_path("prefab_instantiate");

	Origo::JsonSerializer writer(prefab_path);
	writer.write("rid", std::string_view("7"));
	writer.begin_array("native_components");
	writer.begin_array_object();
	writer.write("type", std::string_view("Name"));
	writer.begin_object("data");
	writer.write("name", std::string_view("ImportedPrefab"));
	writer.end_object();
	writer.end_array_object();
	writer.end_array();
	writer.begin_array("script_components");
	writer.end_array();
	writer.save_to_file();

	Origo::JsonSerializer reader(prefab_path);
	reader.load_file();

	Origo::Prefab prefab;
	Origo::PrefabSerializer serializer;
	serializer.deserialize(reader, prefab);

	prefab.backend.begin_array("native_components");
	REQUIRE(prefab.backend.try_begin_array_object_read());

	Origo::JsonSerializer prefab_reader;
	prefab.backend.write_to(prefab_reader);

	Origo::Scene scene("PrefabTest");
	const Origo::RID entity = scene.create_entity("prefab instance");

	Origo::SceneEntitySerializationManager scene_serializer;
	scene_serializer.deserialize_entity_components(scene, entity, prefab_reader);

	auto* name = scene.get_native_component<Origo::NameComponent>(entity);
	REQUIRE(name != nullptr);
	CHECK(name->Name == "ImportedPrefab");

	std::error_code ec;
	std::filesystem::remove(prefab_path, ec);
}
