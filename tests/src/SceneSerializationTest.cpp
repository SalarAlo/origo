#include <doctest/doctest.h>

#include <filesystem>
#include <string>

#include "origo/assets/AssetFactory.h"
#include "origo/assets/Audio.h"
#include "origo/assets/Mesh.h"
#include "origo/assets/model/Model.h"
#include "origo/assets/serialization/SceneSerializer.h"
#include "origo/components/BoxColliderComponent.h"
#include "origo/components/CameraComponent.h"
#include "origo/components/DirectionalLightComponent.h"
#include "origo/components/MeshRendererComponent.h"
#include "origo/components/ModelRendererComponent.h"
#include "origo/components/NameComponent.h"
#include "origo/components/PointLightComponent.h"
#include "origo/components/RigidbodyComponent.h"
#include "origo/components/SoundEmitterComponent.h"
#include "origo/components/TerrainComponent.h"
#include "origo/components/TransformComponent.h"
#include "origo/components/particle_system/ParticleSystemComponent.h"
#include "origo/renderer/MeshRange.h"
#include "origo/scene/Scene.h"
#include "origo/scripting/ComponentRegistry.h"
#include "origo/scripting/ScriptRegistry.h"

namespace {
std::filesystem::path make_temp_path(std::string_view tag) {
	auto base = std::filesystem::temp_directory_path();
	const auto unique = std::to_string(
	    std::filesystem::file_time_type::clock::now().time_since_epoch().count());
	return base / ("origo_" + std::string(tag) + "_" + unique + ".json");
}

Origo::FieldDescriptor make_field(const std::string& owner, const std::string& name, Origo::ReflectedTypeKind kind, Origo::Variant default_value) {
	return Origo::FieldDescriptor {
		.ID = Origo::UUID::from_hash(owner + "::" + name),
		.Name = name,
		.Type = Origo::TypeRef {
			.Kind = kind,
			.Name = name,
		},
		.DefaultValue = std::move(default_value),
	};
}

Origo::ComponentDescriptor make_component(const Origo::UUID& asset_id, const std::string& name, std::vector<Origo::FieldDescriptor> fields) {
	return Origo::ComponentDescriptor {
		.ID = Origo::UUID::from_hash(asset_id.to_string() + "::" + name),
		.AssetID = asset_id,
		.Name = name,
		.Version = 1,
		.Fields = std::move(fields),
	};
}

Origo::ScriptDescriptor make_script(const Origo::UUID& asset_id, const std::string& name, std::vector<Origo::FieldDescriptor> fields) {
	return Origo::ScriptDescriptor {
		.ID = Origo::UUID::from_hash(asset_id.to_string() + "::" + name),
		.AssetID = asset_id,
		.Name = name,
		.Version = 1,
		.Fields = std::move(fields),
	};
}

void check_vec3(const Origo::Vec3& value, float x, float y, float z) {
	CHECK(value.x == doctest::Approx(x));
	CHECK(value.y == doctest::Approx(y));
	CHECK(value.z == doctest::Approx(z));
}

void check_vec4(const glm::vec4& value, float x, float y, float z, float w) {
	CHECK(value.x == doctest::Approx(x));
	CHECK(value.y == doctest::Approx(y));
	CHECK(value.z == doctest::Approx(z));
	CHECK(value.w == doctest::Approx(w));
}
}

TEST_CASE("SceneSerializer roundtrip preserves complex native scene data") {
	const std::filesystem::path json_path = make_temp_path("scene_ser_complex_native");
	auto& factory = Origo::AssetFactory::get_instance();
	const Origo::AssetHandle model_handle = factory.create_runtime_asset<Origo::Model>("RoundtripModel");
	const Origo::AssetHandle mesh_handle = factory.create_runtime_asset<Origo::Mesh>("RoundtripMesh", -1, -1, MeshRange {});
	const Origo::AssetHandle material_handle = factory.create_runtime_asset<Origo::Model>("RoundtripMaterialReference");
	const Origo::AssetHandle audio_handle = factory.create_runtime_asset<Origo::Audio>("RoundtripAudio");

	const auto model_uuid = Origo::AssetManager::get_instance().get_uuid(model_handle);
	const auto mesh_uuid = Origo::AssetManager::get_instance().get_uuid(mesh_handle);
	const auto material_uuid = Origo::AssetManager::get_instance().get_uuid(material_handle);
	const auto audio_uuid = Origo::AssetManager::get_instance().get_uuid(audio_handle);
	REQUIRE(model_uuid.has_value());
	REQUIRE(mesh_uuid.has_value());
	REQUIRE(material_uuid.has_value());
	REQUIRE(audio_uuid.has_value());

	Origo::Scene scene("ComplexRoundtripScene");
	const Origo::RID entity = scene.create_entity("Complex Entity");

	auto* transform = scene.get_native_component<Origo::TransformComponent>(entity);
	REQUIRE(transform != nullptr);
	transform->set_position({ 1.0f, 2.0f, 3.0f });
	transform->set_rotation({ 10.0f, 20.0f, 30.0f });
	transform->set_scale({ 2.0f, 3.0f, 4.0f });

	auto* camera = scene.add_native_component<Origo::CameraComponent>(entity);
	camera->IsPrimary = true;
	camera->CameraObj.set_near(0.25f);
	camera->CameraObj.set_far(420.0f);
	camera->CameraObj.FOV = 67.0f;
	camera->BackgroundColor = { 0.12f, 0.18f, 0.24f };
	camera->PostProcess.Enabled = true;
	camera->PostProcess.ToneMapping.Enabled = true;
	camera->PostProcess.ToneMapping.Exposure = 1.4f;
	camera->PostProcess.ColorAdjustments.Saturation = 0.75f;
	camera->PostProcess.Bloom.Enabled = true;
	camera->PostProcess.Bloom.Intensity = 0.9f;
	camera->PostProcess.Vignette.Color = { 0.1f, 0.2f, 0.3f };
	camera->PostProcess.ChromaticAberration.Enabled = true;
	camera->PostProcess.FilmGrain.Response = 0.6f;

	auto* model_renderer = scene.add_native_component<Origo::ModelRendererComponent>(entity);
	model_renderer->ModelHandle = model_handle;

	auto* mesh_renderer = scene.add_native_component<Origo::MeshRendererComponent>(entity);
	mesh_renderer->MeshHandle = mesh_handle;
	mesh_renderer->MaterialHandle = material_handle;

	auto* directional = scene.add_native_component<Origo::DirectionalLightComponent>(entity);
	directional->Intensity = 3.5f;
	directional->AmbientFactor = 0.15f;
	directional->LightColor = { 0.8f, 0.7f, 0.6f };

	auto* point = scene.add_native_component<Origo::PointLightComponent>(entity);
	point->Intensity = 4.5f;
	point->Color = { 0.4f, 0.5f, 0.6f };
	point->set_attenuation(1.0f, 0.22f, 0.08f);

	auto* rb = scene.add_native_component<Origo::RigidbodyComponent>(entity);
	rb->MotionType = Origo::RigidbodyMotionType::Kinematic;
	rb->Mass = 12.5f;
	rb->Drag = 0.3f;
	rb->AngularDrag = 0.4f;
	rb->Friction = 0.8f;
	rb->Restitution = 0.2f;
	rb->UseGravity = false;
	rb->AllowSleeping = false;
	rb->IsSensor = true;
	rb->MotionQuality = JPH::EMotionQuality::LinearCast;

	auto* collider = scene.add_native_component<Origo::BoxColliderComponent>(entity);
	collider->Offset = { 0.25f, 0.5f, 0.75f };
	collider->Size = { 2.0f, 4.0f, 6.0f };

	auto* sound = scene.add_native_component<Origo::SoundEmitterComponent>(entity);
	sound->AudioHandle = audio_handle;
	sound->IsEnabled = false;
	sound->AutoPlay = false;
	sound->IsLooping = true;
	sound->IsMuted = true;
	sound->Volume = 0.65f;
	sound->Pitch = 1.25f;
	sound->StartDelaySeconds = 2.5f;
	sound->IsSpatial = true;
	sound->FalloffStartDistance = 3.0f;
	sound->FalloffDuration = 12.0f;

	auto* particles = scene.add_native_component<Origo::ParticleSystemComponent>(entity);
	particles->IsLooping = false;
	particles->StartDelay = 0.1f;
	particles->SpawnRate = 44.0f;
	particles->StartSize = 1.2f;
	particles->EndSize = 0.2f;
	particles->LifetimeMin = 2.0f;
	particles->LifetimeMax = 5.0f;
	particles->InitialSpeedMin = 6.0f;
	particles->InitialSpeedMax = 9.0f;
	particles->UseGravity = true;
	particles->GravityForceFactor = 1.7f;
	particles->Drag = 0.12f;
	particles->ParticleMesh = mesh_handle;
	particles->ParticleMaterial = material_handle;
	particles->Bursts = { { 0.5f, 8 }, { 1.5f, 16 } };

	auto* terrain = scene.add_native_component<Origo::TerrainComponent>(entity);
	terrain->style = Origo::TerrainStyle::Voxel;
	terrain->use_texture_layers = true;
	terrain->ground_albedo = material_handle;
	terrain->micro_normal = mesh_handle;
	terrain->size_x = 128;
	terrain->size_z = 256;
	terrain->scale = 0.42f;
	terrain->cell_size = 1.5f;
	terrain->height = 80.0f;
	terrain->contrast = 1.25f;
	terrain->shape_settings.base_strength = 0.77f;
	terrain->surface_settings.snow_start = 0.66f;
	terrain->water_settings.enabled = false;

	Origo::SceneSerializer::serialize_to_file(scene, json_path);
	auto restored = Origo::SceneSerializer::deserialize_from_file(json_path);

	REQUIRE(restored != nullptr);
	CHECK(restored->get_name() == "ComplexRoundtripScene");
	REQUIRE(restored->has_entity(entity));

	const auto* restored_name = restored->get_native_component<Origo::NameComponent>(entity);
	REQUIRE(restored_name != nullptr);
	CHECK(restored_name->Name == "Complex Entity");

	const auto* restored_transform = restored->get_native_component<Origo::TransformComponent>(entity);
	REQUIRE(restored_transform != nullptr);
	check_vec3(restored_transform->get_position(), 1.0f, 2.0f, 3.0f);
	check_vec3(restored_transform->get_rotation(), 10.0f, 20.0f, 30.0f);
	check_vec3(restored_transform->get_scale(), 2.0f, 3.0f, 4.0f);

	const auto* restored_model_renderer = restored->get_native_component<Origo::ModelRendererComponent>(entity);
	REQUIRE(restored_model_renderer != nullptr);
	REQUIRE(restored_model_renderer->ModelHandle.has_value());
	CHECK(Origo::AssetManager::get_instance().get_uuid(*restored_model_renderer->ModelHandle) == model_uuid);

	const auto* restored_mesh_renderer = restored->get_native_component<Origo::MeshRendererComponent>(entity);
	REQUIRE(restored_mesh_renderer != nullptr);
	REQUIRE(restored_mesh_renderer->MeshHandle.has_value());
	REQUIRE(restored_mesh_renderer->MaterialHandle.has_value());
	CHECK(Origo::AssetManager::get_instance().get_uuid(*restored_mesh_renderer->MeshHandle) == mesh_uuid);
	CHECK(Origo::AssetManager::get_instance().get_uuid(*restored_mesh_renderer->MaterialHandle) == material_uuid);

	const auto* restored_camera = restored->get_native_component<Origo::CameraComponent>(entity);
	REQUIRE(restored_camera != nullptr);
	CHECK(restored_camera->IsPrimary);
	CHECK(restored_camera->CameraObj.get_near() == doctest::Approx(0.25f));
	CHECK(restored_camera->CameraObj.get_far() == doctest::Approx(420.0f));
	CHECK(restored_camera->CameraObj.FOV == doctest::Approx(67.0f));
	check_vec3(restored_camera->BackgroundColor, 0.12f, 0.18f, 0.24f);
	CHECK(restored_camera->PostProcess.Enabled);
	CHECK(restored_camera->PostProcess.Bloom.Enabled);
	CHECK(restored_camera->PostProcess.Bloom.Intensity == doctest::Approx(0.9f));
	check_vec3(restored_camera->PostProcess.Vignette.Color, 0.1f, 0.2f, 0.3f);

	const auto* restored_rb = restored->get_native_component<Origo::RigidbodyComponent>(entity);
	REQUIRE(restored_rb != nullptr);
	CHECK(restored_rb->MotionType == Origo::RigidbodyMotionType::Kinematic);
	CHECK(restored_rb->Mass == doctest::Approx(12.5f));
	CHECK(restored_rb->UseGravity == false);
	CHECK(restored_rb->AllowSleeping == false);
	CHECK(restored_rb->IsSensor == true);
	CHECK(restored_rb->MotionQuality == JPH::EMotionQuality::LinearCast);

	const auto* restored_sound = restored->get_native_component<Origo::SoundEmitterComponent>(entity);
	REQUIRE(restored_sound != nullptr);
	REQUIRE(restored_sound->AudioHandle.has_value());
	CHECK(Origo::AssetManager::get_instance().get_uuid(*restored_sound->AudioHandle) == audio_uuid);
	CHECK(restored_sound->IsEnabled == false);
	CHECK(restored_sound->IsLooping == true);
	CHECK(restored_sound->FalloffDuration == doctest::Approx(12.0f));

	const auto* restored_particles = restored->get_native_component<Origo::ParticleSystemComponent>(entity);
	REQUIRE(restored_particles != nullptr);
	CHECK(restored_particles->IsLooping == false);
	CHECK(restored_particles->SpawnRate == doctest::Approx(44.0f));
	REQUIRE(restored_particles->ParticleMesh.has_value());
	REQUIRE(restored_particles->ParticleMaterial.has_value());
	CHECK(Origo::AssetManager::get_instance().get_uuid(*restored_particles->ParticleMesh) == mesh_uuid);
	CHECK(Origo::AssetManager::get_instance().get_uuid(*restored_particles->ParticleMaterial) == material_uuid);
	REQUIRE(restored_particles->Bursts.size() == 2);
	CHECK(restored_particles->Bursts[1].Count == 16);

	const auto* restored_terrain = restored->get_native_component<Origo::TerrainComponent>(entity);
	REQUIRE(restored_terrain != nullptr);
	CHECK(restored_terrain->style == Origo::TerrainStyle::Voxel);
	CHECK(restored_terrain->use_texture_layers);
	CHECK(restored_terrain->size_x == 128);
	CHECK(restored_terrain->shape_settings.base_strength == doctest::Approx(0.77f));
	CHECK(restored_terrain->surface_settings.snow_start == doctest::Approx(0.66f));
	CHECK(restored_terrain->water_settings.enabled == false);
	REQUIRE(restored_terrain->ground_albedo.has_value());
	CHECK(Origo::AssetManager::get_instance().get_uuid(*restored_terrain->ground_albedo) == material_uuid);

	std::error_code ec;
	std::filesystem::remove(json_path, ec);
}

TEST_CASE("SceneSerializer roundtrip preserves reflected vector and reference fields") {
	const std::filesystem::path json_path = make_temp_path("scene_ser_reflected_variants");
	const Origo::UUID asset_id = Origo::UUID::from_hash("scene-serialization-reflected-variant-fields");
	const Origo::UUID referenced_asset = Origo::UUID::from_hash("scene-serialization-reflected-asset-value");
	const Origo::RID referenced_entity = Origo::RID::from_string("123456789");

	const Origo::ComponentTypeID component_id = Origo::ComponentRegistry::register_or_update(make_component(
	    asset_id,
	    "VariantComponent_Test",
	    {
	        make_field("VariantComponent_Test", "mode", Origo::ReflectedTypeKind::Enum, Origo::Variant(0, Origo::VariantType::Enum)),
	        make_field("VariantComponent_Test", "uv", Origo::ReflectedTypeKind::Vec2, Origo::Variant(Origo::Vec2(0.0f))),
	        make_field("VariantComponent_Test", "color", Origo::ReflectedTypeKind::Color, Origo::Variant(glm::vec4(0.0f))),
	        make_field("VariantComponent_Test", "target", Origo::ReflectedTypeKind::Entity, Origo::Variant(Origo::RID {})),
	        make_field("VariantComponent_Test", "asset", Origo::ReflectedTypeKind::Asset, Origo::Variant(Origo::UUID::bad())),
	    }));

	const Origo::ScriptTypeID script_id = Origo::ScriptRegistry::register_or_update(make_script(
	    asset_id,
	    "VariantScript_Test",
	    {
	        make_field("VariantScript_Test", "color", Origo::ReflectedTypeKind::Vec4, Origo::Variant(glm::vec4(0.0f))),
	        make_field("VariantScript_Test", "target", Origo::ReflectedTypeKind::Entity, Origo::Variant(Origo::RID {})),
	    }));

	Origo::Scene scene("ReflectedVariantRoundtripScene");
	const Origo::RID entity = scene.create_entity("Variant Entity");
	scene.add_component_data(entity, component_id);
	scene.add_script(entity, script_id);

	auto* component = scene.get_component_data(entity, component_id);
	REQUIRE(component != nullptr);
	for (auto& field : component->Fields) {
		if (field.Name == "mode")
			field.Value = Origo::Variant(2, Origo::VariantType::Enum);
		else if (field.Name == "uv")
			field.Value = Origo::Variant(Origo::Vec2(0.25f, 0.75f));
		else if (field.Name == "color")
			field.Value = Origo::Variant(glm::vec4(0.1f, 0.2f, 0.3f, 0.4f));
		else if (field.Name == "target")
			field.Value = Origo::Variant(referenced_entity);
		else if (field.Name == "asset")
			field.Value = Origo::Variant(referenced_asset);
	}

	auto* script = scene.get_script(entity, script_id);
	REQUIRE(script != nullptr);
	for (auto& field : script->Fields) {
		if (field.Name == "color")
			field.Value = Origo::Variant(glm::vec4(0.5f, 0.6f, 0.7f, 0.8f));
		else if (field.Name == "target")
			field.Value = Origo::Variant(referenced_entity);
	}

	Origo::SceneSerializer::serialize_to_file(scene, json_path);
	auto restored = Origo::SceneSerializer::deserialize_from_file(json_path);

	REQUIRE(restored != nullptr);
	component = restored->get_component_data(entity, component_id);
	script = restored->get_script(entity, script_id);
	REQUIRE(component != nullptr);
	REQUIRE(script != nullptr);

	for (const auto& field : component->Fields) {
		if (field.Name == "mode") {
			int value {};
			CHECK(field.Value.try_get_as_enum(value));
			CHECK(value == 2);
		} else if (field.Name == "uv") {
			Origo::Vec2 value {};
			CHECK(field.Value.TryGetAsVec2(value));
			CHECK(value.x == doctest::Approx(0.25f));
			CHECK(value.y == doctest::Approx(0.75f));
		} else if (field.Name == "color") {
			glm::vec4 value {};
			CHECK(field.Value.TryGetAsVec4(value));
			check_vec4(value, 0.1f, 0.2f, 0.3f, 0.4f);
		} else if (field.Name == "target") {
			Origo::RID value {};
			CHECK(field.Value.TryGetAsEntity(value));
			CHECK(value == referenced_entity);
		} else if (field.Name == "asset") {
			Origo::UUID value { Origo::UUID::bad() };
			CHECK(field.Value.TryGetAsAsset(value));
			CHECK(value == referenced_asset);
		}
	}

	for (const auto& field : script->Fields) {
		if (field.Name == "color") {
			glm::vec4 value {};
			CHECK(field.Value.TryGetAsVec4(value));
			check_vec4(value, 0.5f, 0.6f, 0.7f, 0.8f);
		} else if (field.Name == "target") {
			Origo::RID value {};
			CHECK(field.Value.TryGetAsEntity(value));
			CHECK(value == referenced_entity);
		}
	}

	std::error_code ec;
	std::filesystem::remove(json_path, ec);
}

TEST_CASE("SceneSerializer roundtrip preserves reflected Noct components") {
	const std::filesystem::path json_path = make_temp_path("scene_ser");
	const Origo::UUID asset_id = Origo::UUID::from_hash("scene-serialization-noct-component");
	auto descriptor = make_component(
	    asset_id,
	    "HealthComponent_Test",
	    {
	        make_field("HealthComponent_Test", "hp", Origo::ReflectedTypeKind::Int, Origo::Variant(100)),
	        make_field("HealthComponent_Test", "label", Origo::ReflectedTypeKind::String, Origo::Variant(std::string("default"))),
	    });

	const Origo::ComponentTypeID component_id = Origo::ComponentRegistry::register_or_update(std::move(descriptor));

	Origo::Scene scene("RoundtripScene");
	const Origo::RID entity = scene.create_entity("Player");
	scene.add_component_data(entity, component_id);

	auto* original_component = scene.get_component_data(entity, component_id);
	REQUIRE(original_component != nullptr);
	REQUIRE(original_component->Fields.size() == 2);
	original_component->Fields[0].Value = Origo::Variant(250);
	original_component->Fields[1].Value = Origo::Variant(std::string("hero"));

	Origo::SceneSerializer::serialize_to_file(scene, json_path);
	auto restored = Origo::SceneSerializer::deserialize_from_file(json_path);

	REQUIRE(restored != nullptr);
	CHECK(restored->get_name() == "RoundtripScene");
	REQUIRE(restored->get_entities().size() == 1);
	CHECK(restored->has_component_data(entity, component_id));

	auto* restored_component = restored->get_component_data(entity, component_id);
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

TEST_CASE("Reflected components from the same script have stable distinct IDs and serialize independently") {
	const std::filesystem::path json_path = make_temp_path("scene_ser_multi_component");
	const Origo::UUID asset_id = Origo::UUID::from_hash("scene-serialization-shared-noct-script");

	const Origo::ComponentTypeID health_id = Origo::ComponentRegistry::register_or_update(make_component(
	    asset_id,
	    "HealthComponent_SharedScript",
	    {
	        make_field("HealthComponent_SharedScript", "value", Origo::ReflectedTypeKind::Int, Origo::Variant(100)),
	        make_field("HealthComponent_SharedScript", "label", Origo::ReflectedTypeKind::String, Origo::Variant(std::string("health"))),
	    }));
	const Origo::ComponentTypeID mana_id = Origo::ComponentRegistry::register_or_update(make_component(
	    asset_id,
	    "ManaComponent_SharedScript",
	    {
	        make_field("ManaComponent_SharedScript", "value", Origo::ReflectedTypeKind::Int, Origo::Variant(50)),
	        make_field("ManaComponent_SharedScript", "enabled", Origo::ReflectedTypeKind::Bool, Origo::Variant(true)),
	    }));

	CHECK(health_id != mana_id);
	const auto& health_desc = Origo::ComponentRegistry::get(health_id);
	const auto& mana_desc = Origo::ComponentRegistry::get(mana_id);
	CHECK(health_desc.AssetID == asset_id);
	CHECK(mana_desc.AssetID == asset_id);
	REQUIRE(health_desc.Fields.size() == 2);
	REQUIRE(mana_desc.Fields.size() == 2);
	CHECK(health_desc.Fields[0].ID != mana_desc.Fields[0].ID);

	Origo::Scene scene("RoundtripSharedScriptScene");
	const Origo::RID entity = scene.create_entity("Caster");
	scene.add_component_data(entity, health_id);
	scene.add_component_data(entity, mana_id);

	auto* health_component = scene.get_component_data(entity, health_id);
	auto* mana_component = scene.get_component_data(entity, mana_id);
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
	CHECK(restored->has_component_data(entity, health_id));
	CHECK(restored->has_component_data(entity, mana_id));

	health_component = restored->get_component_data(entity, health_id);
	mana_component = restored->get_component_data(entity, mana_id);
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

TEST_CASE("Reflected component instances migrate when descriptors are reloaded") {
	const Origo::UUID asset_id = Origo::UUID::from_hash("scene-serialization-reload-noct-component");
	const Origo::ComponentTypeID component_id = Origo::ComponentRegistry::register_or_update(make_component(
	    asset_id,
	    "ReloadableComponent_Test",
	    {
	        make_field("ReloadableComponent_Test", "value", Origo::ReflectedTypeKind::Int, Origo::Variant(10)),
	        make_field("ReloadableComponent_Test", "label", Origo::ReflectedTypeKind::String, Origo::Variant(std::string("first"))),
	    }));

	Origo::Scene scene("ReloadMigrationScene");
	const Origo::RID entity = scene.create_entity("Entity");
	scene.add_component_data(entity, component_id);

	auto* component = scene.get_component_data(entity, component_id);
	REQUIRE(component != nullptr);

	for (auto& field : component->Fields) {
		if (field.Name == "value")
			field.Value = Origo::Variant(99);
	}

	Origo::ComponentRegistry::register_or_update(make_component(
	    asset_id,
	    "ReloadableComponent_Test",
	    {
	        make_field("ReloadableComponent_Test", "value", Origo::ReflectedTypeKind::Int, Origo::Variant(10)),
	        make_field("ReloadableComponent_Test", "enabled", Origo::ReflectedTypeKind::Bool, Origo::Variant(true)),
	    }));

	component = scene.get_component_data(entity, component_id);
	REQUIRE(component != nullptr);
	REQUIRE(component->Fields.size() == 2);

	bool saw_value = false;
	bool saw_enabled = false;

	for (const auto& field : component->Fields) {
		if (field.Name == "value") {
			int value {};
			CHECK(field.Value.TryGetAsInt(value));
			CHECK(value == 99);
			saw_value = true;
		} else if (field.Name == "enabled") {
			bool enabled {};
			CHECK(field.Value.TryGetAsBool(enabled));
			CHECK(enabled == true);
			saw_enabled = true;
		} else {
			FAIL("Unexpected field after reflected component migration");
		}
	}

	CHECK(saw_value);
	CHECK(saw_enabled);
}

TEST_CASE("Reflected component replacement removes components no longer declared by the script") {
	const Origo::UUID asset_id = Origo::UUID::from_hash("scene-serialization-component-removal-noct-script");
	const Origo::ComponentTypeID kept_id = Origo::ComponentRegistry::register_or_update(make_component(
	    asset_id,
	    "KeptComponent_Test",
	    {
	        make_field("KeptComponent_Test", "value", Origo::ReflectedTypeKind::Int, Origo::Variant(10)),
	    }));
	const Origo::ComponentTypeID removed_id = Origo::ComponentRegistry::register_or_update(make_component(
	    asset_id,
	    "RemovedComponent_Test",
	    {
	        make_field("RemovedComponent_Test", "enabled", Origo::ReflectedTypeKind::Bool, Origo::Variant(true)),
	    }));

	Origo::Scene scene("ComponentRemovalScene");
	const Origo::RID entity = scene.create_entity("Entity");
	scene.add_component_data(entity, kept_id);
	scene.add_component_data(entity, removed_id);

	Origo::ComponentRegistry::replace_asset_components(
	    asset_id,
	    {
	        make_component(
	            asset_id,
	            "KeptComponent_Test",
	            {
	                make_field("KeptComponent_Test", "value", Origo::ReflectedTypeKind::Int, Origo::Variant(10)),
	            }),
	    });

	CHECK(scene.has_component_data(entity, kept_id));
	CHECK(!scene.has_component_data(entity, removed_id));
	CHECK(!Origo::ComponentRegistry::exists(removed_id));
}
