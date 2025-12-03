#include "origo/assets/importers/AssetPipeline.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/IAssetImporter.h"
#include <filesystem>

namespace Origo {

// Every importable has a corrosponding .meta and .asset file after this run
// the database is also registered of all of these .meta files
void AssetPipeline::RunInitialImport() {
	auto root { AssetDatabase::ROOT_RESOURCES };

	for (auto& entry : std::filesystem::recursive_directory_iterator(root)) {
		if (!entry.is_regular_file())
			continue;
		const auto& path { entry.path() };

		if (path.extension() == ".meta" || path.extension() == ".asset")
			continue;

		IAssetImporter* importer { AssetImporterRegistry::GetImporter(path) };
		if (!importer)
			continue;

		const std::filesystem::path metaPath = path.string() + ".meta";
		const std::filesystem::path assetPath = path.string() + ".asset";
		AssetMetadata meta;
		if (std::filesystem::exists(metaPath)) {
			meta = AssetDatabase::LoadMetadata(metaPath);
		} else {
			meta.Id = UUID {};
			meta.Name = path.stem().string();
			meta.Type = importer->GetAssetType();
			meta.Origin = AssetOrigin::Imported;
			meta.SourcePath = path;

			AssetDatabase::WriteMetadata(meta);
		}

		AssetDatabase::RegisterMetadata(meta);

		if (!std::filesystem::exists(assetPath)) {
			ORG_INFO("Importing asset: {}", path.string());
			importer->Import(path, meta);
		}
	}

	ORG_INFO("Initial import complete.");
}

}
