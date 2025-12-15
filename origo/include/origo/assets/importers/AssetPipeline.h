#pragma once

#include "origo/assets/AssetDescriptor.h"
#include "origo/assets/importers/IAssetImporter.h"

namespace Origo {

class AssetPipeline {
public:
	static void RunInitialImport();

private:
	static Scope<Metadata> LoadOrCreateMetadata(const std::filesystem::path& path, IAssetImporter* importer);
	static bool CreateAssetFileIfMissing(const std::filesystem::path& path, IAssetImporter* importer, const Metadata& metaData);
};

}
