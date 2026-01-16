#pragma once

#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/Material2D.h"
#include "origo/assets/Shader.h"
#include "origo/components/Component.h"

namespace OrigoEditor {

class EditorOutline : public Origo::Component {
public:
	bool ShouldOutline = false;

public:
	EditorOutline() { }

	std::string GetComponentName() const override { return "EditorSelection"; }

	static Origo::AssetHandle GetOutlineMaterial() {
		using namespace Origo;
		const static Origo::UUID outlineShaderID = Origo::UUID::FromArbitraryString("ENGINE_EDITOR_OUTLINE_SHADER");
		const static Origo::UUID outlineMaterialID = Origo::UUID::FromArbitraryString("ENGINE_EDITOR_OUTLINE_MATERIAL");

		static bool s_Initialised { false };
		if (!s_Initialised) {
			auto shaderHandle = AssetFactory::CreateSyntheticAsset<Shader>("Outline Shader", outlineShaderID);
			auto shader { AssetManager::GetInstance().Get<Shader>(shaderHandle) };

			shader->SetSource(MakeScope<ShaderSourceFile>("resources/shaders/outline.glsl"));
			shader->Resolve();

			m_OutlineMaterial = AssetFactory::CreateSyntheticAsset<Material2D>("Outline Material", outlineMaterialID, shaderHandle);

			s_Initialised = true;
		}

		return *m_OutlineMaterial;
	}

private:
	inline static Origo::OptionalAssetHandle m_OutlineMaterial {};
};

}
