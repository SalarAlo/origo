#pragma once

#include "origo/assets/Asset.h"

namespace Origo {

class SceneAsset : public Asset {
public:
	SceneAsset(const std::optional<std::filesystem::path>& path = std::nullopt)
	    : m_path(path) {
	}

	AssetType get_asset_type() const override { return AssetType::Scene; }
	static AssetType get_class_asset_type() { return AssetType::Scene; }

	const std::optional<std::filesystem::path>& get_path() const { return m_path; }
	void set_path(const std::filesystem::path& path) { m_path = path; }

	void resolve() override {
	}

private:
	std::optional<std::filesystem::path> m_path;
};
}
