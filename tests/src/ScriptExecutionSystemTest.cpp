#include <doctest/doctest.h>

#include "origo/scene/Scene.h"
#include "origo/scripting/ComponentRegistry.h"
#include "origo/scripting/ScriptAttachmentManager.h"
#include "origo/scripting/ScriptExecutionSystem.h"
#include "origo/scripting/ScriptRegistry.h"
#include "origo/scripting/ScriptSystem.h"

TEST_CASE("Noct lifecycle bodies execute control flow through the scripting runtime") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-if-test");
	const std::string source = R"noct(
component RuntimeIfComponent
{
	current : Int = 1;
}

script RuntimeIfScript<RuntimeIfComponent>
{
	fn on_update(dt)
	{
		if current > 0 {
			current = current + 4;
		} else {
			current = 99;
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeIfComponent.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto component_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeIfComponent");
	REQUIRE(component_id.has_value());

	Origo::Scene scene("script-runtime-if-scene");
	const Origo::RID entity = scene.create_entity("Scripted Entity");
	scene.add_component_data(entity, *component_id);
	scene.start_scripts();
	scene.update_scripts(0.016f);

	auto* current = scene.find_component_data_field(entity, *component_id, "current");
	REQUIRE(current != nullptr);

	int value = 0;
	REQUIRE(current->Value.TryGetAsInt(value));
	CHECK(value == 5);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct entity.get_component returns proxies and None for missing components") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-get-component-test");
	const std::string source = R"noct(
component RuntimeProxyHealth
{
	current : Int = 10;
}

component RuntimeProxyMana
{
	current : Int = 5;
}

script RuntimeProxyScript
{
	fn on_update(dt)
	{
		var health = entity.get_component(RuntimeProxyHealth);
		if health != None {
			health.current = health.current - 3;
		}

		var mana = entity.get_component(RuntimeProxyMana);
		if mana == None {
			Log.info("mana is missing");
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeProxyScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto health_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeProxyHealth");
	const auto script_id = Origo::ScriptRegistry::try_find_by_asset_and_name(script_asset, "RuntimeProxyScript");
	REQUIRE(health_id.has_value());
	REQUIRE(script_id.has_value());

	Origo::Scene scene("script-runtime-get-component-scene");
	const Origo::RID entity = scene.create_entity("Scripted Entity");
	scene.add_component_data(entity, *health_id);
	scene.add_script(entity, *script_id);
	scene.start_scripts();
	scene.update_scripts(0.016f);

	auto* current = scene.find_component_data_field(entity, *health_id, "current");
	REQUIRE(current != nullptr);

	int value = 0;
	REQUIRE(current->Value.TryGetAsInt(value));
	CHECK(value == 7);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct direct associated component fields still work with entity component proxies") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-direct-field-with-proxy-test");
	const std::string source = R"noct(
component RuntimeDirectHealth
{
	current : Int = 2;
}

script RuntimeDirectScript<RuntimeDirectHealth>
{
	fn on_update(dt)
	{
		var health = entity.get_component(RuntimeDirectHealth);
		if health != None {
			current = health.current + 5;
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeDirectScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto health_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeDirectHealth");
	REQUIRE(health_id.has_value());

	Origo::Scene scene("script-runtime-direct-field-scene");
	const Origo::RID entity = scene.create_entity("Scripted Entity");
	scene.add_component_data(entity, *health_id);
	scene.start_scripts();
	scene.update_scripts(0.016f);

	auto* current = scene.find_component_data_field(entity, *health_id, "current");
	REQUIRE(current != nullptr);

	int value = 0;
	REQUIRE(current->Value.TryGetAsInt(value));
	CHECK(value == 7);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct runs multiple associated scripts attached to the same entity") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-multiple-systems-test");
	const std::string source = R"noct(
component RuntimeMultiA
{
	value : Int = 1;
}

component RuntimeMultiB
{
	value : Int = 10;
}

script RuntimeMultiASystem<RuntimeMultiA>
{
	fn on_update(dt)
	{
		value = value + 2;
	}
}

script RuntimeMultiBSystem<RuntimeMultiB>
{
	fn on_update(dt)
	{
		value = value + 5;
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeMultiSystems.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto component_a = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeMultiA");
	const auto component_b = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeMultiB");
	REQUIRE(component_a.has_value());
	REQUIRE(component_b.has_value());

	Origo::Scene scene("script-runtime-multiple-systems-scene");
	const Origo::RID entity = scene.create_entity("Scripted Entity");
	scene.add_component_data(entity, *component_a);
	scene.add_component_data(entity, *component_b);
	scene.start_scripts();
	scene.update_scripts(0.016f);

	auto* field_a = scene.find_component_data_field(entity, *component_a, "value");
	auto* field_b = scene.find_component_data_field(entity, *component_b, "value");
	REQUIRE(field_a != nullptr);
	REQUIRE(field_b != nullptr);

	int value_a = 0;
	int value_b = 0;
	REQUIRE(field_a->Value.TryGetAsInt(value_a));
	REQUIRE(field_b->Value.TryGetAsInt(value_b));
	CHECK(value_a == 3);
	CHECK(value_b == 15);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct transform API reads and mutates native transforms") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-transform-api-test");
	const std::string source = R"noct(
component RuntimeTransformProbe
{
	x : Float = 0.0;
	y : Float = 0.0;
	z : Float = 0.0;
	forward_z : Float = 0.0;
}

script RuntimeTransformScript
{
	fn on_update(dt)
	{
		entity.set_position(1.0, 2.0, 3.0);
		entity.translate(4.0, 0.0, -1.0);

		var p = entity.position();
		var f = entity.forward();
		var probe = entity.get_component(RuntimeTransformProbe);
		if probe != None {
			probe.x = p.x;
			probe.y = p.y;
			probe.z = p.z;
			probe.forward_z = f.z;
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeTransformScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto component_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeTransformProbe");
	const auto script_id = Origo::ScriptRegistry::try_find_by_asset_and_name(script_asset, "RuntimeTransformScript");
	REQUIRE(component_id.has_value());
	REQUIRE(script_id.has_value());

	Origo::Scene scene("script-runtime-transform-scene");
	const Origo::RID entity = scene.create_entity("Mover");
	scene.add_component_data(entity, *component_id);
	scene.add_script(entity, *script_id);
	scene.start_scripts();
	scene.update_scripts(0.016f);

	auto* x = scene.find_component_data_field(entity, *component_id, "x");
	auto* y = scene.find_component_data_field(entity, *component_id, "y");
	auto* z = scene.find_component_data_field(entity, *component_id, "z");
	REQUIRE(x != nullptr);
	REQUIRE(y != nullptr);
	REQUIRE(z != nullptr);

	float value = 0.0f;
	REQUIRE(x->Value.TryGetAsFloat(value));
	CHECK(value == doctest::Approx(5.0f));
	REQUIRE(y->Value.TryGetAsFloat(value));
	CHECK(value == doctest::Approx(2.0f));
	REQUIRE(z->Value.TryGetAsFloat(value));
	CHECK(value == doctest::Approx(2.0f));

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct component API supports has add remove for reflected components") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-component-api-test");
	const std::string source = R"noct(
component RuntimeDynamicComponent
{
	value : Int = 0;
}

component RuntimeComponentProbe
{
	value : Int = 0;
}

script RuntimeComponentApiScript
{
	fn on_update(dt)
	{
		var probe = entity.get_component(RuntimeComponentProbe);
		if entity.has_component(RuntimeDynamicComponent) == false {
			var dynamic = entity.add_component(RuntimeDynamicComponent);
			dynamic.value = 42;
		}

		if entity.has_component(RuntimeDynamicComponent) {
			entity.remove_component(RuntimeDynamicComponent);
		}

		if probe != None {
			if entity.has_component(RuntimeDynamicComponent) == false {
				probe.value = 11;
			}
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeComponentApiScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto dynamic_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeDynamicComponent");
	const auto probe_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeComponentProbe");
	const auto script_id = Origo::ScriptRegistry::try_find_by_asset_and_name(script_asset, "RuntimeComponentApiScript");
	REQUIRE(dynamic_id.has_value());
	REQUIRE(probe_id.has_value());
	REQUIRE(script_id.has_value());

	Origo::Scene scene("script-runtime-component-api-scene");
	const Origo::RID entity = scene.create_entity("Component API");
	scene.add_component_data(entity, *probe_id);
	scene.add_script(entity, *script_id);
	scene.start_scripts();
	scene.update_scripts(0.016f);

	CHECK_FALSE(scene.has_component_data(entity, *dynamic_id));
	auto* value = scene.find_component_data_field(entity, *probe_id, "value");
	REQUIRE(value != nullptr);

	int stored = 0;
	REQUIRE(value->Value.TryGetAsInt(stored));
	CHECK(stored == 11);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct scene API finds entities and exposes count get collections") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-scene-api-test");
	const std::string source = R"noct(
component RuntimeSceneMarker
{
	value : Int = 0;
}

component RuntimeSceneProbe
{
	found : Int = 0;
	count : Int = 0;
	missing : Int = 0;
}

script RuntimeSceneApiScript
{
	fn on_update(dt)
	{
		var target = scene.find_entity("Target");
		var probe = entity.get_component(RuntimeSceneProbe);
		if target != None {
			if probe != None {
				probe.found = 1;
			}
		}

		var items = scene.find_entities_with(RuntimeSceneMarker);
		if probe != None {
			probe.count = items.count;
			if scene.find_entity("Missing") == None {
				probe.missing = 1;
			}
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeSceneApiScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto marker_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeSceneMarker");
	const auto probe_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeSceneProbe");
	const auto script_id = Origo::ScriptRegistry::try_find_by_asset_and_name(script_asset, "RuntimeSceneApiScript");
	REQUIRE(marker_id.has_value());
	REQUIRE(probe_id.has_value());
	REQUIRE(script_id.has_value());

	Origo::Scene scene("script-runtime-scene-api-scene");
	const Origo::RID runner = scene.create_entity("Runner");
	const Origo::RID target = scene.create_entity("Target");
	scene.add_component_data(runner, *probe_id);
	scene.add_component_data(target, *marker_id);
	scene.add_script(runner, *script_id);
	scene.start_scripts();
	scene.update_scripts(0.016f);

	int stored = 0;
	auto* found = scene.find_component_data_field(runner, *probe_id, "found");
	auto* count = scene.find_component_data_field(runner, *probe_id, "count");
	auto* missing = scene.find_component_data_field(runner, *probe_id, "missing");
	REQUIRE(found != nullptr);
	REQUIRE(count != nullptr);
	REQUIRE(missing != nullptr);
	REQUIRE(found->Value.TryGetAsInt(stored));
	CHECK(stored == 1);
	REQUIRE(count->Value.TryGetAsInt(stored));
	CHECK(stored == 1);
	REQUIRE(missing->Value.TryGetAsInt(stored));
	CHECK(stored == 1);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct time random input and missing physics APIs are safe in headless scripts") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-service-api-test");
	const std::string source = R"noct(
component RuntimeServiceProbe
{
	delta : Float = 0.0;
	frame : Int = 0;
	chance_one : Int = 0;
	chance_zero : Int = 0;
	no_velocity : Int = 0;
	no_input : Int = 0;
}

script RuntimeServiceApiScript
{
	fn on_update(dt)
	{
		var probe = entity.get_component(RuntimeServiceProbe);
		if probe != None {
			probe.delta = Time.delta();
			probe.frame = Time.frame();
			if Random.chance(1.0) {
				probe.chance_one = 1;
			}
			if Random.chance(0.0) == false {
				probe.chance_zero = 1;
			}
			if entity.velocity() == None {
				probe.no_velocity = 1;
			}
			entity.apply_force(1.0, 0.0, 0.0);
			if Input.key_down("DefinitelyUnknown") == false {
				probe.no_input = 1;
			}
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeServiceApiScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto probe_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeServiceProbe");
	const auto script_id = Origo::ScriptRegistry::try_find_by_asset_and_name(script_asset, "RuntimeServiceApiScript");
	REQUIRE(probe_id.has_value());
	REQUIRE(script_id.has_value());

	Origo::Scene scene("script-runtime-service-api-scene");
	const Origo::RID entity = scene.create_entity("Services");
	scene.add_component_data(entity, *probe_id);
	scene.add_script(entity, *script_id);
	scene.start_scripts();
	scene.update_scripts(0.125f);

	float delta = 0.0f;
	int stored = 0;
	auto* delta_field = scene.find_component_data_field(entity, *probe_id, "delta");
	auto* frame = scene.find_component_data_field(entity, *probe_id, "frame");
	auto* chance_one = scene.find_component_data_field(entity, *probe_id, "chance_one");
	auto* chance_zero = scene.find_component_data_field(entity, *probe_id, "chance_zero");
	auto* no_velocity = scene.find_component_data_field(entity, *probe_id, "no_velocity");
	auto* no_input = scene.find_component_data_field(entity, *probe_id, "no_input");
	REQUIRE(delta_field != nullptr);
	REQUIRE(frame != nullptr);
	REQUIRE(chance_one != nullptr);
	REQUIRE(chance_zero != nullptr);
	REQUIRE(no_velocity != nullptr);
	REQUIRE(no_input != nullptr);
	REQUIRE(delta_field->Value.TryGetAsFloat(delta));
	CHECK(delta == doctest::Approx(0.125f));
	REQUIRE(frame->Value.TryGetAsInt(stored));
	CHECK(stored == 1);
	REQUIRE(chance_one->Value.TryGetAsInt(stored));
	CHECK(stored == 1);
	REQUIRE(chance_zero->Value.TryGetAsInt(stored));
	CHECK(stored == 1);
	REQUIRE(no_velocity->Value.TryGetAsInt(stored));
	CHECK(stored == 1);
	REQUIRE(no_input->Value.TryGetAsInt(stored));
	CHECK(stored == 1);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct collision lifecycle receives other entity proxy") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-collision-payload-test");
	const std::string source = R"noct(
component RuntimeCollisionProbe
{
	value : Int = 0;
}

component RuntimeCollisionOther
{
	value : Int = 7;
}

script RuntimeCollisionScript<RuntimeCollisionProbe>
{
	fn on_collision(other)
	{
		if other != None {
			var other_component = other.get_component(RuntimeCollisionOther);
			if other_component != None {
				value = other_component.value;
			}
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeCollisionScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto probe_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeCollisionProbe");
	const auto other_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeCollisionOther");
	const auto script_id = Origo::ScriptRegistry::try_find_by_asset_and_name(script_asset, "RuntimeCollisionScript");
	REQUIRE(probe_id.has_value());
	REQUIRE(other_id.has_value());
	REQUIRE(script_id.has_value());

	Origo::Scene scene("script-runtime-collision-payload-scene");
	const Origo::RID receiver = scene.create_entity("Receiver");
	const Origo::RID other = scene.create_entity("Other");
	scene.add_component_data(receiver, *probe_id);
	scene.add_component_data(other, *other_id);

	Origo::ScriptAttachmentManager attachments;
	attachments.add(receiver, *script_id);
	Origo::ScriptExecutionSystem scripts;
	scripts.start_scene(scene, attachments);
	scripts.dispatch_event(Origo::ScriptEvent {
	    .EventKind = Origo::ScriptEvent::Kind::Collision,
	    .Receiver = receiver,
	    .Other = other,
	});

	auto* value = scene.find_component_data_field(receiver, *probe_id, "value");
	REQUIRE(value != nullptr);

	int stored = 0;
	REQUIRE(value->Value.TryGetAsInt(stored));
	CHECK(stored == 7);

	scripts.stop_scene();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct reflected Entity fields parse and read as entity proxies") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-entity-field-read-test");
	const std::string source = R"noct(
component RuntimeEntityTarget
{
	target : Entity;
	seen : Int = 0;
	missing : Int = 0;
}

script RuntimeEntityTargetScript<RuntimeEntityTarget>
{
	fn on_update(dt)
	{
		if target != None {
			var p = target.position();
			if p != None {
				seen = 1;
			}
		} else {
			missing = 1;
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeEntityTargetScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto component_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeEntityTarget");
	REQUIRE(component_id.has_value());
	const auto& descriptor = Origo::ComponentRegistry::get(*component_id);
	REQUIRE(descriptor.Fields.size() == 3);
	const auto target_meta = std::find_if(descriptor.Fields.begin(), descriptor.Fields.end(), [](const auto& field) {
		return field.Name == "target";
	});
	REQUIRE(target_meta != descriptor.Fields.end());
	CHECK(target_meta->Type.Kind == Origo::ReflectedTypeKind::Entity);

	Origo::Scene scene("script-runtime-entity-field-read-scene");
	const Origo::RID runner = scene.create_entity("Runner");
	const Origo::RID target = scene.create_entity("Target");
	scene.add_component_data(runner, *component_id);

	auto* target_field = scene.find_component_data_field(runner, *component_id, "target");
	REQUIRE(target_field != nullptr);
	target_field->Value = Origo::Variant(target);

	scene.start_scripts();
	scene.update_scripts(0.016f);

	auto* seen = scene.find_component_data_field(runner, *component_id, "seen");
	REQUIRE(seen != nullptr);

	int stored = 0;
	REQUIRE(seen->Value.TryGetAsInt(stored));
	CHECK(stored == 1);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct reflected Entity fields assign scene entities and clear with None") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-entity-field-write-test");
	const std::string source = R"noct(
component RuntimeEntityWriteTarget
{
	target : Entity;
	step : Int = 0;
}

script RuntimeEntityWriteScript<RuntimeEntityWriteTarget>
{
	fn on_update(dt)
	{
		if step == 0 {
			target = scene.find_entity("Player");
			step = 1;
		} else {
			if target != None {
				target = None;
				step = 2;
			}
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeEntityWriteScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto component_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeEntityWriteTarget");
	REQUIRE(component_id.has_value());

	Origo::Scene scene("script-runtime-entity-field-write-scene");
	const Origo::RID runner = scene.create_entity("Runner");
	const Origo::RID player = scene.create_entity("Player");
	scene.add_component_data(runner, *component_id);
	scene.start_scripts();

	scene.update_scripts(0.016f);
	auto* target = scene.find_component_data_field(runner, *component_id, "target");
	REQUIRE(target != nullptr);
	Origo::RID stored_entity {};
	REQUIRE(target->Value.TryGetAsEntity(stored_entity));
	CHECK(stored_entity == player);

	scene.update_scripts(0.016f);
	REQUIRE(target->Value.TryGetAsEntity(stored_entity));
	CHECK(stored_entity.get_id() < 0);

	auto* step = scene.find_component_data_field(runner, *component_id, "step");
	REQUIRE(step != nullptr);
	int stored = 0;
	REQUIRE(step->Value.TryGetAsInt(stored));
	CHECK(stored == 2);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}

TEST_CASE("Noct component proxy reads and writes reflected Entity fields") {
	const Origo::UUID script_asset = Origo::UUID::from_hash("script-runtime-entity-field-proxy-test");
	const std::string source = R"noct(
component RuntimeProxyEntityField
{
	target : Entity;
	seen : Int = 0;
}

script RuntimeProxyEntityFieldScript
{
	fn on_update(dt)
	{
		var data = entity.get_component(RuntimeProxyEntityField);
		if data != None {
			data.target = scene.find_entity("Player");
			if data.target != None {
				var p = data.target.position();
				if p != None {
					data.seen = 1;
				}
			}
		}
	}
}
)noct";

	Origo::ScriptSystem::register_script(script_asset, "RuntimeProxyEntityFieldScript.noct", source);
	Origo::ScriptSystem::reload_all();

	const auto component_id = Origo::ComponentRegistry::try_find_by_asset_and_name(script_asset, "RuntimeProxyEntityField");
	const auto script_id = Origo::ScriptRegistry::try_find_by_asset_and_name(script_asset, "RuntimeProxyEntityFieldScript");
	REQUIRE(component_id.has_value());
	REQUIRE(script_id.has_value());

	Origo::Scene scene("script-runtime-entity-field-proxy-scene");
	const Origo::RID runner = scene.create_entity("Runner");
	const Origo::RID player = scene.create_entity("Player");
	scene.add_component_data(runner, *component_id);
	scene.add_script(runner, *script_id);
	scene.start_scripts();
	scene.update_scripts(0.016f);

	auto* target = scene.find_component_data_field(runner, *component_id, "target");
	auto* seen = scene.find_component_data_field(runner, *component_id, "seen");
	REQUIRE(target != nullptr);
	REQUIRE(seen != nullptr);

	Origo::RID stored_entity {};
	REQUIRE(target->Value.TryGetAsEntity(stored_entity));
	CHECK(stored_entity == player);

	int stored = 0;
	REQUIRE(seen->Value.TryGetAsInt(stored));
	CHECK(stored == 1);

	scene.stop_scripts();
	Origo::ScriptSystem::shutdown();
}
