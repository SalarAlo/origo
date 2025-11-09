#include "origo/assets/Asset.h"
#include "origo/core/Identifiable.h"
#include <concepts>
#include <unordered_map>

namespace Origo {

template <typename T>
concept AssetConcept = std::derived_from<T, Asset>;
class AssetManager {
private:
	struct Record;

public:
	template <AssetConcept T, typename... Args>
	static T* CreateAsset(const std::string& name, Args&&... args) {
		auto asset { MakeScope<T>(std::forward<Args>(args)...) };
		T* raw = asset.get();
		s_Records[raw->GetId()] = { name, std::move(asset) };

		return raw;
	}

	static const std::unordered_map<UUID, Record>& GetRecords() { return s_Records; }

	static Asset* GetAsset(UUID id);

private:
	struct Record {
		std::string Name {};
		Scope<Asset> AssetReference {};
	};

private:
	inline static std::unordered_map<UUID, Record> s_Records {};
};
}
