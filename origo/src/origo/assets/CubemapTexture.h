#pragma once

#include "origo/assets/Asset.h"
namespace Origo {

struct CubemapDescription {
	std::filesystem::path Right;
	std::filesystem::path Left;
	std::filesystem::path Top;
	std::filesystem::path Bottom;
	std::filesystem::path Front;
	std::filesystem::path Back;

	std::vector<std::filesystem::path> GetPaths() {
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

	AssetType GetAssetType() const override { return AssetType::TextureCubemap; }
	static AssetType GetClassAssetType() { return AssetType::TextureCubemap; }
	void Load();
	void Bind(uint32_t slot) const;

private:
	CubemapDescription m_Description;
	GLuint m_TextureID;
};

}
