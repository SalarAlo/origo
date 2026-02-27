#include <optional>

#include "origo/assets/AssetManager.h"

#include "origo/assets/Asset.h"

namespace Origo {

Asset* AssetManager::get(const AssetHandle& handle) const {
	if (!is_valid(handle))
		return nullptr;

	return m_asset_entries[handle.Index].AssetPtr.get();
}

auto AssetManager::register_asset(Scope<Asset>&& assetPtr, OptionalUUID uuid, OptionalPath path) -> AssetHandle {
	auto handle { get_next_free_handle() };

	m_asset_entries[handle.Index].AssetPtr = std::move(assetPtr);
	m_asset_entries[handle.Index].Uuid = uuid;
	m_asset_entries[handle.Index].Path = path;

	if (uuid.has_value())
		m_uuid_to_handle.insert_or_assign(*uuid, handle);

	return handle;
}

auto AssetManager::get_next_free_handle() -> AssetHandle {
	if (!m_free.empty()) {
		auto i { m_free.back() };
		m_free.pop_back();
		AssetHandle handle { i, ++m_asset_entries[i].Generation };
		return handle;
	} else {
		uint32_t i { static_cast<uint32_t>(m_asset_entries.size()) };
		m_asset_entries.emplace_back(0);
		return { i, 0 };
	}
}

void AssetManager::destroy(const AssetHandle& handle) {
};

bool AssetManager::is_valid(const AssetHandle& handle) const {
	if (handle.Index >= m_asset_entries.size())
		return false;

	const auto& entry = m_asset_entries[handle.Index];

	if (entry.Generation != handle.Generation)
		return false;

	if (!entry.AssetPtr)
		return false;

	return true;
}

OptionalUUID AssetManager::get_uuid(const AssetHandle& handle) const {
	if (!is_valid(handle))
		return std::nullopt;

	return m_asset_entries[handle.Index].Uuid;
}

OptionalAssetHandle AssetManager::get_handle_by_uuid(const UUID& id) const {
	auto it = m_uuid_to_handle.find(id);
	if (it == m_uuid_to_handle.end()) {
		ORG_CORE_WARN("AssetManager::GetHandleByUUID: unknown UUID {}", id.to_string());
		return std::nullopt;
	}

	const AssetHandle& handle = it->second;

	if (!is_valid(handle)) {
		ORG_CORE_WARN("AssetManager::GetHandleByUUID: stale handle for UUID {}", id.to_string());
		return std::nullopt;
	}

	return handle;
}

OptionalAssetHandle AssetManager::load(const std::filesystem::path& path) {
	for (const auto& asset_entry : m_asset_entries) {
		if (!asset_entry.Path || *asset_entry.Path != path || !asset_entry.Uuid.has_value())
			continue;

		auto it = m_uuid_to_handle.find(*asset_entry.Uuid);
		if (it == m_uuid_to_handle.end())
			return std::nullopt;

		return it->second;
	}

	return std::nullopt;
}

void AssetManager::resolve_all(std::optional<std::function<bool(Asset*)>> resolveCond) {
	// need to do this because some assets resolve might resize the entries and they move
	// because of behaviour of std::vector (realloc on capacity exceedence)
	size_t asset_size = m_asset_entries.size();

	for (size_t i = 0; i < asset_size; ++i) {
		auto& entry = m_asset_entries[i];

		if (!entry.AssetPtr)
			continue;
		if (!entry.Uuid.has_value())
			continue;

		if (auto asset_ptr = entry.AssetPtr.get(); resolveCond.has_value() && !((*resolveCond)(asset_ptr)))
			continue;

		std::string path_str = entry.Path ? entry.Path->string() : "<no path>";
		ORG_CORE_TRACE("Resolving asset of type '{}' with path '{}'",
		    magic_enum::enum_name(entry.AssetPtr->get_asset_type()),
		    path_str);

		entry.AssetPtr->resolve();
	}
}

}
