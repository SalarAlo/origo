
#include "SkyboxDefaults.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/CubemapTexture.h"
#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"
#include "origo/assets/SkyboxMaterial.h"

#include "origo/core/PathContext.h"

#include "origo/core/UUID.h"

namespace Origo {

AssetHandle SkyboxDefaults::get_cubemap() {
	static AssetHandle handle = [] {
		const auto skybox_root = PathContextService::get_instance().editor().fallback_root() / "textures" / "skybox";

		CubemapDescription descr {};
		descr.Right = skybox_root / "Right.png";
		descr.Left = skybox_root / "Left.png";
		descr.Top = skybox_root / "Top.png";
		descr.Bottom = skybox_root / "Bottom.png";
		descr.Front = skybox_root / "Front.png";
		descr.Back = skybox_root / "Back.png";

		auto cubemap = AssetFactory::get_instance().get_instance().create_synthetic_asset<CubemapTexture>(
		    "Default Skybox Cubemap",
		    UUID::from_hash("ENGINE_DEFAULT_SKYBOX_CUBEMAP"),
		    descr);

		AssetManager::get_instance()
		    .get_asset<CubemapTexture>(cubemap)
		    ->load();

		return cubemap;
	}();

	return handle;
}

AssetHandle SkyboxDefaults::get_shader() {
	static AssetHandle handle = [] {
		auto shader_handle = AssetFactory::get_instance().get_instance().create_synthetic_asset<Shader>(
		    "Default Skybox Shader",
		    UUID::from_hash("ENGINE_DEFAULT_SKYBOX_SHADER"));

		auto shader { AssetManager::get_instance().get_asset<Shader>(shader_handle) };

		shader->set_source(make_scope<ShaderSourceFile>(
		    PathContextService::get_instance().editor().fallback_root() / "shaders" / "skybox.glsl"));
		shader->resolve();

		return shader_handle;
	}();

	return handle;
}

AssetHandle SkyboxDefaults::get_material() {
	static AssetHandle handle = [this] {
		auto material = AssetFactory::get_instance().get_instance().create_synthetic_asset<SkyboxMaterial>(
		    "Default Skybox Material",
		    UUID::from_hash("ENGINE_DEFAULT_SKYBOX_MATERIAL"),
		    get_shader(),
		    get_cubemap());

		return material;
	}();

	return handle;
}

}
