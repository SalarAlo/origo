#include "origo/assets/importers/AssetPipeline.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/IAssetImporter.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/core/Logger.h"

namespace Origo {

void AssetPipeline::RunInitialImport() {
	std::filesystem::path rootAssetDir { "./" };
	if (!std::filesystem::exists(rootAssetDir)) {
		ORG_WARN("No directory for resources found.");
		return;
	}

	int importCount {};

	for (auto& possibleAsset : std::filesystem::recursive_directory_iterator(rootAssetDir)) {
		if (!possibleAsset.is_regular_file())
			continue;

		const auto& path = possibleAsset.path();

		if (path.extension() == ".import")
			continue;

		IAssetImporter* importer = AssetImporterRegistry::GetImporter(path);
		if (!importer)
			continue;

		auto meta = LoadOrCreateMetadata(path, importer);

		bool needsImport = !std::filesystem::exists(path.string() + ".import");

		if (needsImport) {
			ORG_INFO("Importing asset {}", path.string());

			Scope<Asset> asset = importer->Import(path, *meta);
			if (!asset)
				continue;

			AssetManagerFast::GetInstance().Register(std::move(asset), meta->Id);
			AssetDatabase::RegisterMetadata(*meta);
			AssetDatabase::WriteImportFile(meta->Id);

			importCount++;
		} else {
			AssetDatabase::RegisterMetadata(*meta);
		}
	}

	ORG_INFO("Initial import complete. {} assets imported.", importCount);
}

Scope<Metadata> AssetPipeline::LoadOrCreateMetadata(
    const std::filesystem::path& sourcePath,
    IAssetImporter* importer) {
	const std::filesystem::path importPath { sourcePath.string() + ".import" };

	Scope<Metadata> meta;

	if (std::filesystem::exists(importPath)) {
		meta = MakeScope<Metadata>(
		    AssetDatabase::LoadImportHeader(importPath));
	} else {
		meta = MakeScope<Metadata>();
		meta->Id = UUID::Generate();
		meta->Name = sourcePath.stem().string();
		meta->Type = importer->GetAssetType();
		meta->Origin = AssetOrigin::Imported;
		meta->SourcePath = sourcePath;
	}

	return meta;
}

}
