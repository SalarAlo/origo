
#include "origo/assets/Asset.h"
#include "origo/core/UUID.h"
#include "origo/renderer/Helpers.h"
#include "origo/scripting/ScriptSystem.h"
namespace Origo {

class Script : public Asset {
public:
	Script(const std::filesystem::path& path = "/", const UUID& id = UUID::Generate())
	    : m_Path(path)
	    , m_ID(id) {
		m_AssetType = AssetType::Script;
	}

	AssetType GetAssetType() const override { return AssetType::Script; }
	static AssetType GetClassAssetType() { return AssetType::Script; }

	const std::filesystem::path& GetPath() const { return m_Path; }
	void SetPath(const std::filesystem::path& path) { m_Path = path; }

	void SetID(const UUID& id) { m_ID = id; }
	const UUID& GetID() const { return m_ID; }

	void Resolve() override {
		auto src { ReadFile(m_Path) };
		ScriptSystem::Register(m_ID, m_Path, src);
	}

private:
	std::filesystem::path m_Path;
	UUID m_ID;
};
}
