#include "origo/assets/importers/AssetPipeline.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/IAssetImporter.h"
#include "origo/assets/AssetMetadata.h"
#include <filesystem>

namespace Origo {

// Every importable has a corrosponding .meta and .asset file after this run
// the database is also registered of all of these .meta files
void AssetPipeline::RunInitialImport() {
	auto rootAssetDir { AssetDatabase::ROOT_RESOURCES };

	int importCount {};
	int assetFileCreationCount {};
	for (auto& possibleAsset : std::filesystem::recursive_directory_iterator(rootAssetDir)) {
		if (!possibleAsset.is_regular_file())
			continue;

		const auto& path { possibleAsset.path() };

		if (path.extension() == ".meta" || path.extension() == ".asset")
			continue;

		IAssetImporter* importer { AssetImporterRegistry::GetImporter(path) };
		if (!importer) {
			ORG_INFO("No importer for file of extension {}", path.extension().string());
			continue;
		}

		auto meta { LoadOrCreateMetadata(path, importer) };
		importCount++;

		if (CreateAssetFileIfMissing(path, importer, *meta))
			assetFileCreationCount++;

		AssetDatabase::RegisterMetadata(*meta);
	}

	ORG_INFO("Initial import complete. {} .meta files created/registered. {} .asset files ceated", importCount, assetFileCreationCount);
}

Scope<AssetMetadata> AssetPipeline::LoadOrCreateMetadata(const std::filesystem::path& path, IAssetImporter* importer) {
	const std::filesystem::path metaPath { path.string() + ".meta" };
	Scope<AssetMetadata> meta { nullptr };
	if (std::filesystem::exists(metaPath)) {
		meta = std::move(MakeScope<AssetMetadata>(AssetDatabase::LoadMetadata(metaPath)));
	} else {
		meta = std::move(MakeScope<AssetMetadata>());
		meta->Id = UUID {};
		meta->Name = path.stem().string();
		meta->Type = importer->GetAssetType();
		meta->Origin = AssetOrigin::Imported;
		meta->SourcePath = path;

		AssetDatabase::WriteMetadata(*meta);
	}

	return meta;
}

bool AssetPipeline::CreateAssetFileIfMissing(const std::filesystem::path& path, IAssetImporter* importer, const AssetMetadata& meta) {
	const std::filesystem::path assetPath = path.string() + ".asset";

	if (!std::filesystem::exists(assetPath)) {
		ORG_INFO("Creating .asset file for: {}", path.string());
		importer->Import(path, meta);
		ORG_INFO("Finished Creating .asset file for: {}", path.string());
		return true;
	}

	return false;
}

}
