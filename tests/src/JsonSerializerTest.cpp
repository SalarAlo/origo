#include <doctest/doctest.h>

#include "origo/serialization/JsonSerializer.h"

TEST_CASE("JsonSerializer copy preserves nested readable state") {
	Origo::JsonSerializer source;
	source.write("rid", std::string_view("42"));
	source.begin_array("native_components");
	source.begin_array_object();
	source.write("type", std::string_view("Name"));
	source.begin_object("data");
	source.write("name", std::string_view("PrefabCube"));
	source.end_object();
	source.end_array_object();
	source.end_array();
	source.begin_array("script_components");
	source.end_array();

	Origo::JsonSerializer copied;
	copied = source;

	std::string rid;
	REQUIRE(copied.try_read("rid", rid));
	CHECK(rid == "42");

	copied.begin_array("native_components");
	REQUIRE(copied.try_begin_array_object_read());

	std::string type;
	REQUIRE(copied.try_read("type", type));
	CHECK(type == "Name");

	copied.begin_object("data");
	std::string name;
	REQUIRE(copied.try_read("name", name));
	CHECK(name == "PrefabCube");
	copied.end_object();

	copied.end_array_object();
	copied.end_array();
}
