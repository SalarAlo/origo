#include <doctest/doctest.h>

#include <filesystem>
#include <string>

#include "origo/assets/serialization/SceneSerializer.h"
#include "origo/scene/Scene.h"
#include "origo/scripting/ScriptComponentDescriptor.h"
#include "origo/scripting/ScriptComponentRegistry.h"

#include "sol/state.hpp"

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

TEST_CASE("Lua-defined script components from the same script have stable distinct IDs and serialize independently") {
	const std::filesystem::path json_path = make_temp_path("scene_ser_multi_script");
	const Origo::UUID script_id = Origo::UUID::from_hash("scene-serialization-shared-script");

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::table);

	sol::table health_fields = lua.create_table();
	health_fields["value"] = lua.create_table_with(
	    "type", "int",
	    "default", 100);
	health_fields["label"] = lua.create_table_with(
	    "type", "string",
	    "default", "health");

	sol::table mana_fields = lua.create_table();
	mana_fields["value"] = lua.create_table_with(
	    "type", "int",
	    "default", 50);
	mana_fields["enabled"] = lua.create_table_with(
	    "type", "bool",
	    "default", true);

	const Origo::ScriptComponentID health_id = Origo::ScriptComponentRegistry::register_component_from_lua(
	    script_id,
	    "HealthComponent_SharedScript",
	    health_fields);
	const Origo::ScriptComponentID mana_id = Origo::ScriptComponentRegistry::register_component_from_lua(
	    script_id,
	    "ManaComponent_SharedScript",
	    mana_fields);

	CHECK(health_id != mana_id);

	const auto& health_desc = Origo::ScriptComponentRegistry::get(health_id);
	const auto& mana_desc = Origo::ScriptComponentRegistry::get(mana_id);
	CHECK(health_desc.ScriptID == script_id);
	CHECK(mana_desc.ScriptID == script_id);
	REQUIRE(health_desc.Fields.size() == 2);
	REQUIRE(mana_desc.Fields.size() == 2);
	CHECK(health_desc.Fields[0].ID != mana_desc.Fields[0].ID);

	Origo::Scene scene("RoundtripSharedScriptScene");
	const Origo::RID entity = scene.create_entity("Caster");
	scene.add_script_component(entity, health_id);
	scene.add_script_component(entity, mana_id);

	auto* health_component = scene.get_script_component(entity, health_id);
	auto* mana_component = scene.get_script_component(entity, mana_id);
	REQUIRE(health_component != nullptr);
	REQUIRE(mana_component != nullptr);

	for (auto& field : health_component->Fields) {
		if (field.Name == "value")
			field.Value = Origo::Variant(275);
		else if (field.Name == "label")
			field.Value = Origo::Variant(std::string("hero-health"));
	}

	for (auto& field : mana_component->Fields) {
		if (field.Name == "value")
			field.Value = Origo::Variant(80);
		else if (field.Name == "enabled")
			field.Value = Origo::Variant(false);
	}

	Origo::SceneSerializer::serialize_to_file(scene, json_path);
	auto restored = Origo::SceneSerializer::deserialize_from_file(json_path);

	REQUIRE(restored != nullptr);
	CHECK(restored->has_script_component(entity, health_id));
	CHECK(restored->has_script_component(entity, mana_id));

	health_component = restored->get_script_component(entity, health_id);
	mana_component = restored->get_script_component(entity, mana_id);
	REQUIRE(health_component != nullptr);
	REQUIRE(mana_component != nullptr);

	int int_value {};
	std::string string_value;
	bool bool_value = true;

	for (const auto& field : health_component->Fields) {
		if (field.Name == "value") {
			CHECK(field.Value.TryGetAsInt(int_value));
			CHECK(int_value == 275);
		} else if (field.Name == "label") {
			CHECK(field.Value.TryGetAsString(string_value));
			CHECK(string_value == "hero-health");
		}
	}

	for (const auto& field : mana_component->Fields) {
		if (field.Name == "value") {
			CHECK(field.Value.TryGetAsInt(int_value));
			CHECK(int_value == 80);
		} else if (field.Name == "enabled") {
			CHECK(field.Value.TryGetAsBool(bool_value));
			CHECK(bool_value == false);
		}
	}

	std::error_code ec;
	std::filesystem::remove(json_path, ec);
}
