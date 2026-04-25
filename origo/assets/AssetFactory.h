#pragma once

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Metadata.h"

#include "origo/core/UUID.h"

#include "origo/utils/Singleton.h"

namespace Origo {

class AssetFactory : public Singleton<AssetFactory> {
public:
	using AssetCreatedCallback = std::function<void(UUID, AssetType)>;

public:
	template <typename T, typename... Args>
	AssetHandle create_runtime_asset(const std::string& name, Args&&... args) {
		AssetMetadata meta {};

		meta.Name = name;
		meta.ID = UUID::generate();
		meta.Type = T::get_class_asset_type();
		meta.Origin = AssetOrigin::Runtime;

		AssetDatabase::get_instance().register_metadata(meta);

		auto asset = make_scope<T>(std::forward<Args>(args)...);
		return AssetManager::get_instance().register_asset(std::move(asset), meta.ID);
	}

	template <typename T, typename... Args>
	AssetHandle create_runtime_asset_with_parent(const std::string& name, const UUID& parent_id, Args&&... args) {
		AssetMetadata meta {};

		meta.Name = name;
		meta.ID = UUID::generate();
		meta.ParentID = parent_id;
		meta.Type = T::get_class_asset_type();
		meta.Origin = AssetOrigin::Runtime;

		AssetDatabase::get_instance().register_metadata(meta);

		auto asset = make_scope<T>(std::forward<Args>(args)...);
		return AssetManager::get_instance().register_asset(std::move(asset), meta.ID);
	}

	template <typename T, typename... Args>
	AssetHandle create_pathed_runtime_asset(const std::string& name, const std::filesystem::path& path, Args&&... args) {
		AssetMetadata meta {};

		meta.SourcePath = path;
		meta.Name = name;
		meta.ID = UUID::generate();
		meta.Type = T::get_class_asset_type();
		meta.Origin = AssetOrigin::Runtime;

		AssetDatabase::get_instance().register_metadata(meta);

		auto asset = make_scope<T>(std::forward<Args>(args)...);
		return AssetManager::get_instance().register_asset(std::move(asset), meta.ID);
	}

	template <typename T, typename... Args>
	AssetHandle create_pathed_runtime_asset_with_parent(const std::string& name, const std::filesystem::path& path, const UUID& parent_id, Args&&... args) {
		AssetMetadata meta {};

		meta.SourcePath = path;
		meta.Name = name;
		meta.ID = UUID::generate();
		meta.ParentID = parent_id;
		meta.Type = T::get_class_asset_type();
		meta.Origin = AssetOrigin::Runtime;

		AssetDatabase::get_instance().register_metadata(meta);

		auto asset = make_scope<T>(std::forward<Args>(args)...);
		return AssetManager::get_instance().register_asset(std::move(asset), meta.ID);
	}

	template <typename T, typename... Args>
	AssetHandle create_synthetic_asset(const std::string& name, const UUID& uuid, Args&&... args) {
		AssetMetadata meta {};
		meta.Name = name;
		meta.ID = uuid;
		meta.Type = T::get_class_asset_type();
		meta.Origin = AssetOrigin::Synthetic;

		AssetDatabase::get_instance().register_metadata(meta);

		auto asset = make_scope<T>(std::forward<Args>(args)...);
		return AssetManager::get_instance().register_asset(std::move(asset), uuid);
	}

	AssetHandle create_imported_asset(const AssetMetadata& meta, Scope<Asset>&& asset) {
		AssetDatabase::get_instance().register_metadata(meta);

		return AssetManager::get_instance().register_asset(
		    std::move(asset),
		    meta.ID,
		    meta.SourcePath);
	}

	Scope<Asset> allocate_hollow_asset(AssetType type);

	template <AssetConcept T>
	Scope<T> allocate_hollow_asset() {
		return make_scope<T>();
	}
};

}
