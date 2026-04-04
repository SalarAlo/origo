#include "origo/assets/serialization/MaterialSerializer.h"

#include "origo/assets/AssetManager.h"

#include "origo/assets/material/MaterialPBR.h"
#include "origo/assets/material/MaterialPBRSource.h"

#include "origo/renderer/PBRParameters.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void MaterialSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	auto material {
		static_cast<const MaterialPBR*>(asset)
	};

	backend.write("shader", material->get_data().ShaderHandle);

	backend.begin_object("textures");
	material->get_data().PBRTexs.serialize(backend);
	backend.end_object();

	backend.begin_object("parameters");
	material->get_data().PBRParams.serialize(backend);
	backend.end_object();

	material->get_uniform_list().serialize(backend);
}

void MaterialSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	auto& material = static_cast<MaterialPBR&>(asset);
	auto& deps = material.get_deps();

	if (std::string shader_id; backend.try_read("shader", shader_id)) {
		deps.Shader = UUID::from_string(shader_id);
	}

	backend.begin_object("textures");
	material.get_deps().merge_missing(PBRTextures::deserialize_to_deps(backend));
	backend.end_object();

	backend.begin_object("parameters");
	material.get_data().PBRParams = PBRParameters::deserialize(backend);
	backend.end_object();

	material.get_uniform_list().deserialize(backend);

	auto& am = AssetManager::get_instance();
	if (deps.Shader) {
		auto shader_handle = am.get_handle_by_uuid(*deps.Shader);
		if (shader_handle)
			material.get_data().ShaderHandle = *shader_handle;
	}
	if (deps.Albedo) {
		auto albedo_handle = am.get_handle_by_uuid(*deps.Albedo);
		if (albedo_handle)
			material.get_data().PBRTexs.Albedo = *albedo_handle;
	}
	if (deps.Normal) {
		auto normal_handle = am.get_handle_by_uuid(*deps.Normal);
		if (normal_handle)
			material.get_data().PBRTexs.Normal = *normal_handle;
	}
	if (deps.MetallicRoughness) {
		auto metallic_roughness_handle = am.get_handle_by_uuid(*deps.MetallicRoughness);
		if (metallic_roughness_handle)
			material.get_data().PBRTexs.MetallicRoughness = *metallic_roughness_handle;
	}
	if (deps.Ao) {
		auto ao_handle = am.get_handle_by_uuid(*deps.Ao);
		if (ao_handle)
			material.get_data().PBRTexs.Ao = *ao_handle;
	}
	if (deps.Emissive) {
		auto emissive_handle = am.get_handle_by_uuid(*deps.Emissive);
		if (emissive_handle)
			material.get_data().PBRTexs.Emissive = *emissive_handle;
	}
}

}
