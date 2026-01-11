#pragma once

#include "origo/assets/Asset.h"

namespace Origo {

class SceneAsset : public Asset {
public:
	SceneAsset(const std::optional<std::filesystem::path>& path = std::nullopt)
	    : m_Path(path) {
		m_AssetType = AssetType::Scene;
	}

	AssetType GetAssetType() const override { return AssetType::Scene; }
	static AssetType GetClassAssetType() { return AssetType::Scene; }

	const std::optional<std::filesystem::path>& GetPath() const { return m_Path; }
	void SetPath(const std::filesystem::path& path) { m_Path = path; }

	void Resolve() override {
	}

private:
	std::optional<std::filesystem::path> m_Path;
};
}
