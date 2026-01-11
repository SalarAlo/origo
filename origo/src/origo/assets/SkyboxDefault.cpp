
#include "SkyboxDefaults.h"

#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/CubemapTexture.h"
#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"
#include "origo/assets/SkyboxMaterial.h"
#include "origo/core/UUID.h"

namespace Origo {

AssetHandle SkyboxDefaults::GetCubemap() {
	static AssetHandle handle = [] {
		CubemapDescription descr {};
		descr.Right = "./resources/textures/skybox/Right.png";
		descr.Left = "./resources/textures/skybox/Left.png";
		descr.Top = "./resources/textures/skybox/Top.png";
		descr.Bottom = "./resources/textures/skybox/Bottom.png";
		descr.Front = "./resources/textures/skybox/Front.png";
		descr.Back = "./resources/textures/skybox/Back.png";

		auto cubemap = AssetFactory::CreateSyntheticAsset<CubemapTexture>(
		    "Default Skybox Cubemap",
		    UUID::FromHash("ENGINE_DEFAULT_SKYBOX_CUBEMAP"),
		    descr);

		AssetManager::GetInstance()
		    .Get<CubemapTexture>(cubemap)
		    ->Load();

		return cubemap;
	}();

	return handle;
}

AssetHandle SkyboxDefaults::GetShader() {
	static AssetHandle handle = [] {
		auto shaderHandle = AssetFactory::CreateSyntheticAsset<Shader>(
		    "Default Skybox Shader",
		    UUID::FromHash("ENGINE_DEFAULT_SKYBOX_SHADER"));

		auto shader { AssetManager::GetInstance().Get<Shader>(shaderHandle) };

		shader->SetSource(MakeScope<ShaderSourceFile>(
		    "./resources/shaders/skybox.glsl"));
		shader->Resolve();

		return shaderHandle;
	}();

	return handle;
}

AssetHandle SkyboxDefaults::GetMaterial() {
	static AssetHandle handle = [] {
		auto material = AssetFactory::CreateSyntheticAsset<SkyboxMaterial>(
		    "Default Skybox Material",
		    UUID::FromHash("ENGINE_DEFAULT_SKYBOX_MATERIAL"),
		    GetShader(),
		    GetCubemap());

		return material;
	}();

	return handle;
}

}
