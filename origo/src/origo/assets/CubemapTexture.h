#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
namespace Origo {

struct CubemapDescription {
	std::filesystem::path Right;
	std::filesystem::path Left;
	std::filesystem::path Top;
	std::filesystem::path Bottom;
	std::filesystem::path Front;
	std::filesystem::path Back;

	std::vector<std::filesystem::path> get_paths() {
		std::vector<std::filesystem::path> paths {
			Right,
			Left,
			Top,
			Bottom,
			Front,
			Back
		};
		return paths;
	}
};

class CubemapTexture : public Asset {
public:
	CubemapTexture(CubemapDescription description);

	AssetType get_asset_type() const override { return AssetType::TextureCubemap; }
	static AssetType get_class_asset_type() { return AssetType::TextureCubemap; }
	void load();
	void bind(uint32_t slot) const;

private:
	CubemapDescription m_description;
	GLuint m_texture_id;
};

}
