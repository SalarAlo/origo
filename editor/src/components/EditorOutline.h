#pragma once

#include "origo/assets/AssetManager.h"

#include "origo/components/Component.h"

namespace OrigoEditor {

class EditorOutlineComponent : public Origo::Component {
public:
	bool ShouldOutline = false;

public:
	EditorOutlineComponent() { }

	std::string GetComponentName() const override { return "EditorSelection"; }

private:
	inline static Origo::OptionalAssetHandle m_OutlineMaterial {};
};

}
