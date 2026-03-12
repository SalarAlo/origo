#include <doctest/doctest.h>

#include <filesystem>
#include <string>

#include "origo/assets/serialization/SceneSerializer.h"
#include "origo/scene/Scene.h"
#include "origo/scripting/ScriptComponentDescriptor.h"
#include "origo/scripting/ScriptComponentRegistry.h"

namespace {
std::filesystem::path make_temp_path(std::string_view tag) {
	auto base = std::filesystem::temp_directory_path();
	const auto unique = std::to_string(
	    std::filesystem::file_time_type::clock::now().time_since_epoch().count());
	return base / ("origo_" + std::string(tag) + "_" + unique + ".json");
}
}

TEST_CASE("SceneSerializer roundtrip preserves scripted components") {
	const std::filesystem::path json_path = make_temp_path("scene_ser");

	Origo::ScriptComponentDescriptor descriptor {};
	descriptor.ID = Origo::UUID::from_hash("scene-serialization-script-component");
	descriptor.Name = "HealthComponent_Test";
	descriptor.Fields = {
		{
		    Origo::UUID::from_hash("scene-serialization-script-component::hp"),
		    "hp",
		    Origo::VariantType::Int,
		    Origo::Variant(100),
		},
		{
		    Origo::UUID::from_hash("scene-serialization-script-component::label"),
		    "label",
		    Origo::VariantType::String,
		    Origo::Variant(std::string("default")),
		},
	};

	const Origo::ScriptComponentID component_id = Origo::ScriptComponentRegistry::register_or_update(descriptor);

	Origo::Scene scene("RoundtripScene");
	const Origo::RID entity = scene.create_entity("Player");
	scene.add_script_component(entity, component_id);

	auto* original_component = scene.get_script_component(entity, component_id);
	REQUIRE(original_component != nullptr);
	REQUIRE(original_component->Fields.size() == 2);
	original_component->Fields[0].Value = Origo::Variant(250);
	original_component->Fields[1].Value = Origo::Variant(std::string("hero"));

	Origo::SceneSerializer::serialize_to_file(scene, json_path);
	auto restored = Origo::SceneSerializer::deserialize_from_file(json_path);

	REQUIRE(restored != nullptr);
	CHECK(restored->get_name() == "RoundtripScene");
	REQUIRE(restored->get_entities().size() == 1);
	CHECK(restored->has_script_component(entity, component_id));

	auto* restored_component = restored->get_script_component(entity, component_id);
	REQUIRE(restored_component != nullptr);
	REQUIRE(restored_component->Fields.size() == 2);

	int hp {};
	std::string label;
	CHECK(restored_component->Fields[0].Value.TryGetAsInt(hp));
	CHECK(hp == 250);
	CHECK(restored_component->Fields[1].Value.TryGetAsString(label));
	CHECK(label == "hero");

	std::error_code ec;
	std::filesystem::remove(json_path, ec);
}
