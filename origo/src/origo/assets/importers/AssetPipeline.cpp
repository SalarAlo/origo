#include "origo/assets/importers/AssetPipeline.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/core/Logger.h"
#include "origo/serialization/JsonSerializer.h"
#include <filesystem>

namespace Origo {

void AssetImportPipeline::RunInitialImport() {
	std::filesystem::path rootAssetDir { "./resources" };

	if (!std::filesystem::exists(rootAssetDir)) {
		ORG_CORE_WARN("No directory for resources found.");
		return;
	}

	int importCount {};

	for (auto& assetFile : std::filesystem::recursive_directory_iterator(rootAssetDir)) {
		if (!IsImportCandidate(assetFile))
			continue;

		const auto& assetPath = assetFile.path();

		IAssetImporter* assetImporter = ResolveImporter(assetPath);
		if (!assetImporter)
			continue;

		auto meta = LoadOrCreateMetadata(assetPath, assetImporter);

		if (IsImportNecessary(assetPath, *meta)) {
			ImportAsset(assetPath, assetImporter, *meta, importCount);
		} else {
			LoadCachedAsset(assetPath.string() + ".import", *meta);
		}
	}

	ORG_CORE_TRACE("Initial import complete. {} assets imported.", importCount);

	s_InitialImportComplete = true;

	AssetManager::GetInstance().ResolveAll();
}

bool AssetImportPipeline::IsImportCandidate(const std::filesystem::directory_entry& entry) {
	if (!entry.is_regular_file())
		return false;

	return entry.path().extension() != ".import";
}

IAssetImporter* AssetImportPipeline::ResolveImporter(const std::filesystem::path& path) {
	return AssetImporterRegistry::GetImporter(path);
}

bool AssetImportPipeline::IsImportNecessary(const std::filesystem::path& path, const AssetMetadata& meta) {
	std::filesystem::path importFile { path.string() + ".import" };

	if (!std::filesystem::exists(importFile))
		return true;

	if (!meta.SourcePath.empty() && meta.ImportedTimestamp < meta.SourceTimestamp)
		return true;

	return false;
}

void AssetImportPipeline::ImportAsset(const std::filesystem::path& path, IAssetImporter* importer, AssetMetadata& meta, int& importCount) {
	Scope<Asset> asset = importer->Import(path, meta);

	AssetManager::GetInstance().Register(std::move(asset), meta.ID);
	meta.ImportedTimestamp = meta.SourceTimestamp;

	AssetDatabase::RegisterMetadata(meta);
	AssetDatabase::WriteImportFile(meta.ID);

	importCount++;
}

void AssetImportPipeline::LoadCachedAsset(const std::filesystem::path& importFile, AssetMetadata& meta) {
	auto assetSerializer = AssetSerializationSystem::Get(meta.Type);

	JsonSerializer serializer { importFile.c_str() };
	serializer.LoadFile();

	auto asset = AssetFactory::AllocateHollowAsset(meta.Type);

	ORG_CORE_TRACE("Beginning deserilaization for {} of type {} and path {}", meta.Name, magic_enum::enum_name(meta.Type), meta.SourcePath.c_str());

	serializer.BeginObject("payload");
	assetSerializer->Deserialize(serializer, *asset.get());
	serializer.EndObject();

	AssetFactory::CreateImportedAsset(meta, std::move(asset));
	AssetDatabase::RegisterMetadata(meta);
}

Scope<AssetMetadata> AssetImportPipeline::LoadOrCreateMetadata(const std::filesystem::path& sourcePath, IAssetImporter* importer) {
	const std::filesystem::path importPath { sourcePath.string() + ".import" };
	Scope<AssetMetadata> meta;

	if (std::filesystem::exists(importPath)) {
		meta = MakeScope<AssetMetadata>(AssetDatabase::LoadImportHeader(importPath));
	} else {
		meta = MakeScope<AssetMetadata>();
		meta->ID = UUID::Generate();
		meta->Name = sourcePath.stem().string();
		meta->Type = importer->GetAssetType();
		meta->Origin = AssetOrigin::Imported;
		meta->SourcePath = sourcePath;

		if (std::filesystem::exists(sourcePath))
			meta->SourceTimestamp = std::filesystem::last_write_time(sourcePath);

		using sys = std::chrono::system_clock;
		meta->ImportedTimestamp = std::chrono::clock_cast<std::filesystem::file_time_type::clock>(
		    sys::from_time_t(0));
	}

	return meta;
}

}
