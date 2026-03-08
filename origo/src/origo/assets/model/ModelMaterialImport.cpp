#include <assimp/material.h>
#include <assimp/scene.h>
#include <cstdlib>

#include "Model.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/TextureSource.h"

namespace Origo {

OptionalAssetHandle Model::try_load_material_texture(
    const aiScene* scene,
    aiMaterial* ai_mat,
    aiTextureType ai_texture_type,
    TextureType texture_type,
    const std::filesystem::path& model_parent_path,
    const std::string& debug_name) {
	if (!scene || !ai_mat)
		return std::nullopt;

	aiString tex_path;
	if (ai_mat->GetTexture(ai_texture_type, 0, &tex_path) != AI_SUCCESS)
		return std::nullopt;

	const std::string path = tex_path.C_Str();
	const auto model_id = AssetManager::get_instance().get_uuid(this);
	const auto texture_handle = model_id
	    ? AssetFactory::get_instance().create_runtime_asset_with_parent<Texture2D>(
	          debug_name,
	          *model_id,
	          texture_type)
	    : AssetFactory::get_instance().create_runtime_asset<Texture2D>(
	          debug_name,
	          texture_type);

	auto texture = AssetManager::get_instance().get_asset<Texture2D>(texture_handle);
	if (!texture)
		return std::nullopt;

	if (!path.empty() && path[0] == '*') {
		const int texture_index = std::atoi(path.c_str() + 1);
		if (texture_index >= 0 && texture_index < (int)scene->mNumTextures) {
			aiTexture* embedded = scene->mTextures[texture_index];
			if (embedded) {
				if (embedded->mHeight == 0) {
					std::vector<unsigned char> compressed(
					    reinterpret_cast<unsigned char*>(embedded->pcData),
					    reinterpret_cast<unsigned char*>(embedded->pcData) + embedded->mWidth);
					texture->set_source(make_scope<TextureSourceEmbedded>(std::move(compressed)));
				} else {
					const int width = embedded->mWidth;
					const int height = embedded->mHeight;
					std::vector<unsigned char> pixels((size_t)width * height * 4);
					const aiTexel* source = reinterpret_cast<const aiTexel*>(embedded->pcData);

					for (int pixel = 0; pixel < width * height; ++pixel) {
						pixels[pixel * 4 + 0] = source[pixel].r;
						pixels[pixel * 4 + 1] = source[pixel].g;
						pixels[pixel * 4 + 2] = source[pixel].b;
						pixels[pixel * 4 + 3] = source[pixel].a;
					}

					texture->set_source(make_scope<TextureSourceEmbedded>(width, height, 4, std::move(pixels)));
				}
			}
		}
	} else {
		texture->set_source(make_scope<TextureSourceFile>((model_parent_path / path).string()));
	}

	texture->set_owner(this);
	texture->load();

	return OptionalAssetHandle { texture_handle };
}

}
