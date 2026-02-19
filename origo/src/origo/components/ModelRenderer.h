#pragma once

#include "Component.h"

#include "origo/assets/AssetManager.h"

namespace Origo {

class ModelRendererComponent : public Component {
public:
	ModelRendererComponent() = default;
	ModelRendererComponent(const AssetHandle& handle)
	    : ModelHandle(handle) { };

	std::string GetComponentName() const override { return "ModelRenderer"; }

	OptionalAssetHandle ModelHandle {};
};

}
