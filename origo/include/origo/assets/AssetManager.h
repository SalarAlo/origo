#include "nlohmann/json_fwd.hpp"
#include "origo/assets/Asset.h"
#include "origo/core/Identifiable.h"
namespace Origo {

class AssetManager {
public:
	template <typename T, typename... Args>
	static Ref<T> CreateAsset(const std::string& name, Args&&... args) {
		static_assert(std::is_base_of_v<Asset, T>);

		auto asset = MakeRef<T>(std::forward<Args>(args)...);
		s_Assets[asset->GetId()] = asset;
		s_AssetNames[asset->GetId()] = name;

		return asset;
	}

	static Ref<Asset> GetAsset(UUID id) {
		auto it = s_Assets.find(id);
		return it != s_Assets.end() ? it->second : nullptr;
	}

	static void RegisterAsset(UUID id, const Ref<Asset>& asset) {
		s_Assets[id] = asset;
	}

	static nlohmann::json SaveAll(); // serialize to disk (later)
	static nlohmann::json LoadAll(); // deserialize (later)

private:
	inline static std::unordered_map<UUID, Ref<Asset>> s_Assets {};
	inline static std::unordered_map<UUID, std::string> s_AssetNames {};
};
}
