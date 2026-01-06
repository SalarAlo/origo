#pragma once

#include <ankerl/unordered_dense.h>
#include <limits>
#include <optional>

#include "origo/assets/Asset.h"
#include "origo/core/UUID.h"

namespace Origo {

template <typename T>
concept AssetConcept = std::derived_from<T, Asset> && requires(T t) {
	{ t.GetClassAssetType() } -> std::same_as<AssetType>;
};

using OptionalPath = std::optional<std::filesystem::path>;

struct AssetHandle {
	AssetHandle() = default;
	AssetHandle(uint32_t index, uint32_t generation)
	    : Generation(generation)
	    , Index(index) { };

	uint32_t Generation { 0 };
	uint32_t Index { std::numeric_limits<uint32_t>::max() };

	constexpr bool IsNull() const { return Index == std::numeric_limits<uint32_t>::max(); }
	bool operator==(const AssetHandle& other) const { return other.Index == Index && other.Generation == Generation; }
	bool operator!=(const AssetHandle& other) const { return !(other == *this); }
};

struct AssetEntry {
	AssetEntry(uint32_t generation)
	    : Generation(generation) { };
	uint32_t Generation;

	Scope<Asset> AssetPtr {};
	UUID Uuid { UUID::Bad() };
	OptionalPath Path { std::nullopt };
};

class AssetManager {

public:
	static AssetManager& GetInstance();

	template <AssetConcept T>
	T* Get(const AssetHandle& handle) {
		if (!IsValid(handle))
			return nullptr;

		Asset* base = m_AssetEntries[handle.Index].AssetPtr.get();
		if (base->GetAssetType() != T::GetClassAssetType())
			return nullptr;

		return static_cast<T*>(m_AssetEntries[handle.Index].AssetPtr.get());
	}

	Asset* Get(const AssetHandle& handle) const;
	AssetHandle Register(Scope<Asset>&& assetPtr, UUID uuid = UUID::Bad(), OptionalPath path = std::nullopt);
	void Destroy(const AssetHandle& handle);
	bool IsValid(const AssetHandle& handle) const;
	UUID GetUUID(const AssetHandle& handle) const;
	AssetHandle GetHandleByUUID(const UUID& uuid) const;
	void ResolveAll(std::optional<std::function<bool(Asset*)>> = std::nullopt);
	const ankerl::unordered_dense::map<UUID, AssetHandle>& GetUuidMap() { return m_UuidToHandle; }

	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager(AssetManager&&) = delete;
	AssetManager& operator=(AssetManager&&) = delete;

private:
	AssetManager() = default;
	AssetHandle GetNextFreeHandle();

private:
	std::vector<AssetEntry> m_AssetEntries {};
	std::vector<uint32_t> m_Free {};
	ankerl::unordered_dense::map<UUID, AssetHandle> m_UuidToHandle;
};
}
