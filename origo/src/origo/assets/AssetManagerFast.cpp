#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Asset.h"

namespace Origo {

AssetManagerFast& AssetManagerFast::GetInstance() {
	static AssetManagerFast s_Instance {};
	return s_Instance;
}
Asset* AssetManagerFast::Get(const AssetHandle& handle) const {
	if (!IsValid(handle))
		return nullptr;

	return m_Entries[handle.Index].AssetPtr.get();
}

auto AssetManagerFast::Register(Scope<Asset>&& assetPtr, UUID uuid, OptionalPath path) -> AssetHandle {
	auto handle { GetNextFreeHandle() };

	m_Entries[handle.Index].AssetPtr = std::move(assetPtr);
	m_Entries[handle.Index].Uuid = uuid;
	m_Entries[handle.Index].Path = path;

	m_UuidToHandle[uuid] = handle;

	return handle;
}

auto AssetManagerFast::GetNextFreeHandle() -> AssetHandle {
	if (!m_Free.empty()) {
		auto i { m_Free.back() };
		m_Free.pop_back();
		AssetHandle handle { i, ++m_Entries[i].Generation };
		return handle;
	} else {
		uint32_t i { static_cast<uint32_t>(m_Entries.size()) };
		m_Entries.emplace_back(0);
		return { i, 0 };
	}
}

void AssetManagerFast::Destroy(const AssetHandle& handle) {
};

bool AssetManagerFast::IsValid(const AssetHandle& handle) const {
	bool isInBounds { m_Entries.size() > handle.Index };
	if (!isInBounds)
		return false;

	bool isGenerationsMatch { m_Entries[handle.Index].Generation == handle.Generation };
	return isGenerationsMatch;
}

UUID AssetManagerFast::GetUUID(const AssetHandle& handle) const {
	if (!IsValid(handle))
		return UUID::Bad();
	return m_Entries[handle.Index].Uuid;
}

AssetHandle AssetManagerFast::Resolve(const UUID& id) const {
	if (id == UUID::Bad())
		return {};

	auto it = m_UuidToHandle.find(id);
	if (it == m_UuidToHandle.end()) {
		ORG_ERROR("AssetManager::Resolve: unknown UUID {}", id.ToString());
		return {};
	}

	const AssetHandle& handle = it->second;

	if (!IsValid(handle)) {
		ORG_ERROR("AssetManager::Resolve: stale handle for UUID {}", id.ToString());
		return {};
	}

	return handle;
}

void AssetManagerFast::ResolveAll() {
	for (const auto& entry : m_Entries) {
		if (entry.AssetPtr) {
			entry.AssetPtr->Resolve();
		}
	}
}

}
