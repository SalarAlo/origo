#include "origo/assets/serialization/MaterialSerializer.h"

#include "origo/assets/material/MaterialPBR.h"
#include "origo/assets/material/MaterialPBRSource.h"

#include "origo/assets/AssetManager.h"

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

	// Backward compatibility for legacy .mat schema:
	// {
	//   "color_r/g/b", "data": { "shader", "albedo" }
	// }
	if (!deps.Shader) {
		backend.begin_object("data");
		if (std::string legacy_shader_id; backend.try_read("shader", legacy_shader_id)) {
			deps.Shader = UUID::from_string(legacy_shader_id);
		}
		if (std::string legacy_albedo_id; backend.try_read("albedo", legacy_albedo_id)) {
			deps.Albedo = UUID::from_string(legacy_albedo_id);
		}
		backend.end_object();
	}

	Origo::Vec3 legacy_color = material.get_data().PBRParams.BaseColor;
	const bool has_r = backend.try_read("color_r", legacy_color.r);
	const bool has_g = backend.try_read("color_g", legacy_color.g);
	const bool has_b = backend.try_read("color_b", legacy_color.b);
	if (has_r && has_g && has_b) {
		material.get_data().PBRParams.BaseColor = legacy_color;
	}

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
}

}
