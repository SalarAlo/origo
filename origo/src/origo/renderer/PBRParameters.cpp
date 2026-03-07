#include <exception>
#include <string_view>

#include "origo/renderer/PBRParameters.h"

#include "origo/assets/material/MaterialDeps.h"

#include "origo/core/UUID.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {
namespace {
	void write_optional_asset_uuid(ISerializer& backend, std::string_view key, const OptionalAssetHandle& handle) {
		if (!handle.has_value())
			return;

		auto uuid = AssetManager::get_instance().get_uuid(*handle);
		if (!uuid.has_value())
			return;

		backend.write(key, uuid->to_string());
	}

	OptionalAssetHandle read_optional_asset_uuid(ISerializer& backend, std::string_view key) {
		std::string uuid_str {};
		if (!backend.try_read(key, uuid_str))
			return std::nullopt;

		try {
			return AssetManager::get_instance().get_handle_by_uuid(UUID::from_string(uuid_str));
		} catch (const std::exception&) {
			return std::nullopt;
		}
	}

	OptionalUUID read_optional_uuid(ISerializer& backend, std::string_view key) {
		std::string uuid_str {};
		if (!backend.try_read(key, uuid_str))
			return std::nullopt;
		auto uuid { UUID::from_string(uuid_str) };
		return uuid;
	}

}

void PBRParameters::serialize(ISerializer& backend) const {
	backend.write("base_color", BaseColor);
	backend.write("metallic", Metallic);
	backend.write("roughness", Roughness);
	backend.write("ao", AO);
	backend.write("unlit", static_cast<int>(Unlit));
}

PBRParameters PBRParameters::deserialize(ISerializer& backend) {
	PBRParameters parameters {};

	backend.try_read("base_color", parameters.BaseColor);
	backend.try_read("metallic", parameters.Metallic);
	backend.try_read("roughness", parameters.Roughness);
	backend.try_read("ao", parameters.AO);
	int unlit = parameters.Unlit ? 1 : 0;
	if (backend.try_read("unlit", unlit))
		parameters.Unlit = unlit != 0;

	return parameters;
}

void PBRTextures::serialize(ISerializer& backend) const {
	write_optional_asset_uuid(backend, "albedo", Albedo);
	write_optional_asset_uuid(backend, "normal", Normal);
	write_optional_asset_uuid(backend, "metallic_roughness", MetallicRoughness);
	write_optional_asset_uuid(backend, "ao", Ao);
	write_optional_asset_uuid(backend, "emissive", Emissive);
}

PBRTextures PBRTextures::deserialize(ISerializer& backend) {
	PBRTextures textures {};

	textures.Albedo = read_optional_asset_uuid(backend, "albedo");
	textures.Normal = read_optional_asset_uuid(backend, "normal");
	textures.MetallicRoughness = read_optional_asset_uuid(backend, "metallic_roughness");
	textures.Ao = read_optional_asset_uuid(backend, "ao");
	textures.Emissive = read_optional_asset_uuid(backend, "emissive");

	return textures;
}

MaterialDeps PBRTextures::deserialize_to_deps(ISerializer& backend) {
	MaterialDeps deps;

	deps.Albedo = read_optional_uuid(backend, "albedo");
	deps.Normal = read_optional_uuid(backend, "normal");
	deps.MetallicRoughness = read_optional_uuid(backend, "metallic_roughness");
	deps.Ao = read_optional_uuid(backend, "ao");
	deps.Emissive = read_optional_uuid(backend, "emissive");

	return deps;
}

}
