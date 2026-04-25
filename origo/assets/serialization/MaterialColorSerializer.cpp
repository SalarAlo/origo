#include "origo/assets/serialization/MaterialColorSerializer.h"

#include "origo/assets/AssetManager.h"

#include "origo/assets/material/MaterialColor.h"

namespace Origo {

void MaterialColorSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	auto material {
		static_cast<const MaterialColor*>(asset)
	};

	backend.write("shader", material->get_shader_handle());

	backend.begin_object("parameters");
	backend.write("unlit", material->get_data().Unlit);
	backend.end_object();

	material->get_uniform_list().serialize(backend);
}

void MaterialColorSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	auto& material = static_cast<MaterialColor&>(asset);
	auto& deps = material.get_deps();

	if (std::string shader_id; backend.try_read("shader", shader_id))
		deps.Shader = UUID::from_string(shader_id);

	backend.begin_object("parameters");
	backend.try_read("unlit", material.get_data().Unlit);
	backend.end_object();

	material.get_uniform_list().deserialize(backend);

	auto& am = AssetManager::get_instance();
	if (deps.Shader) {
		auto shader_handle = am.get_handle_by_uuid(*deps.Shader);
		if (shader_handle)
			material.set_shader(*shader_handle);
	}
}

}
