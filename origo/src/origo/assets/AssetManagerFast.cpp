#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Asset.h"
#include <optional>

namespace Origo {

AssetManager& AssetManager::GetInstance() {
	static AssetManager s_Instance {};
	return s_Instance;
}

Asset* AssetManager::Get(const AssetHandle& handle) const {
	if (!IsValid(handle))
		return nullptr;

	return m_AssetEntries[handle.Index].AssetPtr.get();
}

auto AssetManager::Register(Scope<Asset>&& assetPtr, OptionalUUID uuid, OptionalPath path) -> AssetHandle {
	auto handle { GetNextFreeHandle() };

	m_AssetEntries[handle.Index].AssetPtr = std::move(assetPtr);
	m_AssetEntries[handle.Index].Uuid = uuid;
	m_AssetEntries[handle.Index].Path = path;

	if (uuid.has_value())
		m_UuidToHandle.insert_or_assign(*uuid, handle);

	return handle;
}

auto AssetManager::GetNextFreeHandle() -> AssetHandle {
	if (!m_Free.empty()) {
		auto i { m_Free.back() };
		m_Free.pop_back();
		AssetHandle handle { i, ++m_AssetEntries[i].Generation };
		return handle;
	} else {
		uint32_t i { static_cast<uint32_t>(m_AssetEntries.size()) };
		m_AssetEntries.emplace_back(0);
		return { i, 0 };
	}
}

void AssetManager::Destroy(const AssetHandle& handle) {
};

bool AssetManager::IsValid(const AssetHandle& handle) const {
	bool isInBounds { m_AssetEntries.size() > handle.Index };
	if (!isInBounds)
		return false;

	bool isGenerationsMatch { m_AssetEntries[handle.Index].Generation == handle.Generation };
	return isGenerationsMatch;
}

OptionalUUID AssetManager::GetUUID(const AssetHandle& handle) const {
	if (!IsValid(handle))
		return std::nullopt;

	return m_AssetEntries[handle.Index].Uuid;
}

OptionalAssetHandle AssetManager::GetHandleByUUID(const UUID& id) const {
	auto it = m_UuidToHandle.find(id);
	if (it == m_UuidToHandle.end()) {
		ORG_ERROR("AssetManager::GetHandleByUUID: unknown UUID {}", id.ToString());
		return std::nullopt;
	}

	const AssetHandle& handle = it->second;

	if (!IsValid(handle)) {
		ORG_ERROR("AssetManager::GetHandleByUUID: stale handle for UUID {}", id.ToString());
		return std::nullopt;
	}

	return handle;
}

OptionalAssetHandle AssetManager::Load(const std::filesystem::path& path) {
	for (const auto& assetEntry : m_AssetEntries) {
		if (!assetEntry.Path || *assetEntry.Path != path || !assetEntry.Uuid.has_value())
			continue;

		auto it = m_UuidToHandle.find(*assetEntry.Uuid);
		if (it == m_UuidToHandle.end())
			return std::nullopt;

		return it->second;
	}

	return std::nullopt;
}

void AssetManager::ResolveAll(std::optional<std::function<bool(Asset*)>> resolveCond) {
	// need to do this because some assets resolve might resize the entries and they move
	// because of behaviour of std::vector (realloc on capacity exceedence)
	size_t assetSize = m_AssetEntries.size();

	for (size_t i = 0; i < assetSize; ++i) {
		auto& entry = m_AssetEntries[i];

		if (!entry.AssetPtr)
			continue;
		if (!entry.Uuid.has_value())
			continue;

		if (auto assetPtr = entry.AssetPtr.get(); resolveCond.has_value() && !((*resolveCond)(assetPtr)))
			continue;

		std::string pathStr = entry.Path ? entry.Path->string() : "<no path>";
		ORG_INFO("Resolving asset of type '{}' with path '{}'",
		    magic_enum::enum_name(entry.AssetPtr->GetAssetType()),
		    pathStr);

		entry.AssetPtr->Resolve();
	}
}

}
