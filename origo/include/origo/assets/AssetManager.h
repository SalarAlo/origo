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

	static Ref<T> CreateAsset(const std::string& name, Args&&... args) {
		auto asset = MakeRef<T>(std::forward<Args>(args)...);
		s_Records[asset->GetId()] = { name, asset };

		return asset;
	}

	static const std::unordered_map<UUID, Record>& GetRecords() { return s_Records; }

	static Ref<Asset> GetAsset(UUID id);

private:
	struct Record {
		std::string Name {};
		Ref<Asset> AssetReference {};
	};

private:
	inline static std::unordered_map<UUID, Record> s_Records {};
};
}
