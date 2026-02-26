#pragma once

#include "AssetManager.h"

#include "origo/utils/Singleton.h"

namespace Origo {

class DefaultAssetCache : public Singleton<DefaultAssetCache> {
public:
	void create_all_defaults();

	AssetHandle get_shader();
	AssetHandle get_texture();

	AssetHandle get_material();
	AssetHandle get_outline_material();
	AssetHandle get_particle_emission_debug_material();
	AssetHandle get_particle_material();

private:
	std::optional<AssetHandle> m_shader;
	std::optional<AssetHandle> m_texture;
	std::optional<AssetHandle> m_material;
	std::optional<AssetHandle> m_outline_material;
	std::optional<AssetHandle> m_particle_emission_debug_material;
	std::optional<AssetHandle> m_particle_material;
};

}
