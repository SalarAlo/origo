#pragma once

#include "origo/core/UUID.h"

namespace Origo {
struct MaterialDeps {
	OptionalUUID Shader;
	OptionalUUID Albedo;
	OptionalUUID Normal;
	OptionalUUID MetallicRoughness;
	OptionalUUID Ao;
	OptionalUUID Emissive;

	void merge_missing(const MaterialDeps& other) {
		if (!Shader)
			Shader = other.Shader;

		if (!Albedo)
			Albedo = other.Albedo;

		if (!Normal)
			Normal = other.Normal;

		if (!MetallicRoughness)
			MetallicRoughness = other.MetallicRoughness;

		if (!Ao)
			Ao = other.Ao;

		if (!Emissive)
			Emissive = other.Emissive;
	}

	void merge(const MaterialDeps& other) {
		if (other.Shader)
			Shader = other.Shader;

		if (other.Albedo)
			Albedo = other.Albedo;

		if (other.Normal)
			Normal = other.Normal;

		if (other.MetallicRoughness)
			MetallicRoughness = other.MetallicRoughness;

		if (other.Ao)
			Ao = other.Ao;

		if (other.Emissive)
			Emissive = other.Emissive;
	}
};

}
