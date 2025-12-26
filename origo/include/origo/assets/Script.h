
#include "origo/assets/Asset.h"
#include "origo/core/UUID.h"
#include "origo/scripting/ScriptSystem.h"
namespace Origo {

class Script : public Asset {
public:
	Script(std::string source = "", const UUID& id = UUID::Generate())
	    : m_Source(std::move(source))
	    , m_ID(id) {
		m_AssetType = AssetType::Script;
	}

	AssetType GetAssetType() const override { return AssetType::Script; }
	static AssetType GetClassAssetType() { return AssetType::Script; }

	const std::string& GetSource() const { return m_Source; }
	void SetSource(const std::string& source) { m_Source = source; }

	void SetID(const UUID& id) { m_ID = id; }
	const UUID& GetID() const { return m_ID; }

	void Resolve() override {
		ScriptSystem::Register(m_ID, m_Source);
	}

private:
	std::string m_Source;
	UUID m_ID;
};
}
