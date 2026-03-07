#pragma once

#include <ankerl/unordered_dense.h>

#include <limits>
#include <optional>

#include "origo/assets/Asset.h"

#include "origo/core/UUID.h"

#include "origo/utils/Singleton.h"

namespace Origo {

template <typename T>
concept AssetConcept = std::derived_from<T, Asset> && requires(T t) {
	{ t.get_class_asset_type() } -> std::same_as<AssetType>;
};

using OptionalPath = std::optional<std::filesystem::path>;

struct AssetHandle {
	AssetHandle(uint32_t index, uint32_t generation)
	    : Generation(generation)
	    , Index(index) { };

	uint32_t Generation { 0 };
	uint32_t Index { std::numeric_limits<uint32_t>::max() };

	bool operator==(const AssetHandle& other) const { return other.Index == Index && other.Generation == Generation; }
	bool operator!=(const AssetHandle& other) const { return !(other == *this); }
};

using OptionalAssetHandle = std::optional<AssetHandle>;

struct AssetEntry {
	AssetEntry(uint32_t generation)
	    : Generation(generation) { };
	uint32_t Generation;

	Scope<Asset> AssetPtr {};
	OptionalUUID Uuid { std::nullopt };
	OptionalPath Path { std::nullopt };
};

class AssetManager : public Singleton<AssetManager> {
public:
	template <AssetConcept T>
	T* get_asset(const AssetHandle& handle) {
		if (!is_valid(handle))
			return nullptr;

		Asset* base = m_asset_entries[handle.Index].AssetPtr.get();
		if (base->get_asset_type() != T::get_class_asset_type())
			return nullptr;

		return static_cast<T*>(m_asset_entries[handle.Index].AssetPtr.get());
	}

	Asset* get(const AssetHandle& handle) const;
	AssetHandle register_asset(Scope<Asset>&& assetPtr, OptionalUUID uuid = std::nullopt, OptionalPath path = std::nullopt);
	void destroy(const AssetHandle& handle);
	bool is_valid(const AssetHandle& handle) const;
	OptionalUUID get_uuid(const AssetHandle& handle) const;
	OptionalUUID get_uuid(const Asset* asset) const;
	OptionalAssetHandle get_handle_by_uuid(const UUID& uuid) const;
	void resolve_all(std::optional<std::function<bool(Asset*)>> = std::nullopt);
	const ankerl::unordered_dense::map<UUID, AssetHandle>& get_uuid_map() { return m_uuid_to_handle; }
	OptionalAssetHandle load(const std::filesystem::path& path);

private:
	AssetHandle get_next_free_handle();

private:
	std::vector<AssetEntry> m_asset_entries {};
	std::vector<uint32_t> m_free {};
	ankerl::unordered_dense::map<UUID, AssetHandle> m_uuid_to_handle;
};
}
