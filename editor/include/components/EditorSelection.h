#pragma once

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace OrigoEditor {

class EditorSelection : public Origo::Component {
public:
	bool IsSelected = false;

public:
	EditorSelection() { }

	std::string GetName() const override { return "EditorSelection"; }

	static Origo::AssetHandle GetOutlineMaterial() {
		using namespace Origo;

		static bool s_Initialised { false };
		if (!s_Initialised) {
			auto shader = AssetFactory::CreateAsset<Shader>("Outline Shader");
			AssetManagerFast::GetInstance().Get<Shader>(shader)->SetSource(MakeScope<ShaderSourceFile>("resources/shaders/outline.glsl"));

			m_OutlineMaterial = AssetFactory::CreateAsset<Material>("Outline Shader", shader);

			s_Initialised = true;
		}

		return m_OutlineMaterial;
	}

private:
	inline static Origo::AssetHandle m_OutlineMaterial {};
};

}
