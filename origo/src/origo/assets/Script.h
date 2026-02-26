#pragma once

#include "origo/assets/Asset.h"

#include "origo/core/UUID.h"

#include "origo/renderer/Helpers.h"

#include "origo/scripting/ScriptSystem.h"

namespace Origo {

class Script : public Asset {
public:
	Script(const std::filesystem::path& path = "/", const UUID& id = UUID::generate())
	    : m_path(path)
	    , m_id(id) {
		m_asset_type = AssetType::Script;
	}

	AssetType get_asset_type() const override { return AssetType::Script; }
	static AssetType get_class_asset_type() { return AssetType::Script; }

	const std::filesystem::path& get_path() const { return m_path; }
	void set_path(const std::filesystem::path& path) { m_path = path; }

	void set_id(const UUID& id) { m_id = id; }
	const UUID& get_id() const { return m_id; }

	void resolve() override {
		auto src { read_file(m_path) };
		ScriptSystem::register_script(m_id, m_path, src);
	}

private:
	std::filesystem::path m_path;
	UUID m_id;
};
}
