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

class AssetManagerFast {
public:
	using OptionalPath = std::optional<std::filesystem::path>;

	struct AssetHandle {
		AssetHandle() = default;
		AssetHandle(uint32_t index, uint32_t generation)
		    : Generation(generation)
		    , Index(index) { };

		uint32_t Generation { 0 };
		uint32_t Index { std::numeric_limits<uint32_t>::max() };

		constexpr bool IsNull() const { return Index == std::numeric_limits<uint32_t>::max(); }
	};

	struct AssetEntry {
		AssetEntry(uint32_t generation)
		    : Generation(generation) { };
		uint32_t Generation;

		Scope<Asset> AssetPtr {};
		UUID Uuid { UUID::Bad() };
		OptionalPath Path { std::nullopt };
	};

public:
	static AssetManagerFast& GetInstance();

	template <AssetConcept T>
	T* Get(const AssetHandle& handle) {
		if (!IsValid(handle))
			return nullptr;

		Asset* base = m_Entries[handle.Index].AssetPtr.get();
		if (base->GetAssetType() != T::GetClassAssetType())
			return nullptr;

		return static_cast<T*>(m_Entries[handle.Index].AssetPtr.get());
	}

	Asset* Get(const AssetHandle& handle) const;
	AssetHandle Register(Scope<Asset>&& assetPtr, UUID uuid = UUID::Bad(), OptionalPath path = std::nullopt);
	void Destroy(const AssetHandle& handle);
	bool IsValid(const AssetHandle& handle) const;

	AssetManagerFast(const AssetManagerFast&) = delete;
	AssetManagerFast& operator=(const AssetManagerFast&) = delete;
	AssetManagerFast(AssetManagerFast&&) = delete;
	AssetManagerFast& operator=(AssetManagerFast&&) = delete;

private:
	AssetManagerFast() = default;
	AssetHandle GetNextFreeHandle();

private:
	std::vector<AssetEntry> m_Entries {};
	std::vector<uint32_t> m_Free {};
};
}
