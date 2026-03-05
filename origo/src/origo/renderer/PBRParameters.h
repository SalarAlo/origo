#pragma once

#include "origo/assets/AssetManager.h"

#include "origo/assets/material/MaterialDeps.h"

namespace Origo {

class ISerializer;

struct PBRParameters {
	Vec3 BaseColor { 1.0f };
	float Metallic { 0.0f };
	float Roughness { 1.0f };
	float AO { 1.0f };

	void serialize(ISerializer& backend) const;
	static PBRParameters deserialize(ISerializer& backend);
};

struct PBRTextures {
	OptionalAssetHandle Albedo;
	OptionalAssetHandle Normal;
	OptionalAssetHandle MetallicRoughness;
	OptionalAssetHandle Ao;
	OptionalAssetHandle Emissive;

	void serialize(ISerializer& backend) const;
	static PBRTextures deserialize(ISerializer& backend);
	static MaterialDeps deserialize_to_deps(ISerializer& backend);
};

}
