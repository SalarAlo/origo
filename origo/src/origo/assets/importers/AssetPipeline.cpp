#include "origo/assets/importers/AssetPipeline.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/IAssetImporter.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/core/Logger.h"
#include "origo/serialization/JsonSerializer.h"
#include <filesystem>

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

		std::filesystem::path importFile { path.string() + ".import" };
		bool importNecessary = !std::filesystem::exists(importFile) || !meta->SourcePath.empty() && meta->ImportedTimestamp < meta->SourceTimestamp;

		if (importNecessary) {
			ORG_INFO("Importing asset {}", path.string());

			Scope<Asset> asset = importer->Import(path, *meta);
			if (!asset)
				continue;

			AssetManagerFast::GetInstance().Register(std::move(asset), meta->ID);
			meta->ImportedTimestamp = meta->SourceTimestamp;
			AssetDatabase::RegisterMetadata(*meta);
			AssetDatabase::WriteImportFile(meta->ID);

			importCount++;
		} else {
			auto assetSerializer = AssetSerializationSystem::Get(meta->Type);

			JsonSerializer serializer { importFile.c_str() };
			serializer.LoadFile();

			auto asset { AssetFactory::Create(meta->Type) };

			ORG_INFO("Beginning deserilaization for {} of type {} and path {}", meta->Name, magic_enum::enum_name(meta->Type), meta->SourcePath.c_str());

			serializer.BeginObject("payload");
			assetSerializer->Deserialize(serializer, *asset.get());
			serializer.EndObject();

			AssetManagerFast::GetInstance().Register(std::move(asset), meta->ID);
			AssetDatabase::RegisterMetadata(*meta);
		}
	}

	ORG_INFO("Initial import complete. {} assets imported.", importCount);
	AssetManagerFast::GetInstance().ResolveAll();
}

Scope<AssetMetadata> AssetPipeline::LoadOrCreateMetadata(
    const std::filesystem::path& sourcePath,
    IAssetImporter* importer) {
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
		sys::time_point unixEpoch = sys::from_time_t(0);
		meta->ImportedTimestamp = std::chrono::clock_cast<std::filesystem::file_time_type::clock>(unixEpoch);
	}

	return meta;
}

}
