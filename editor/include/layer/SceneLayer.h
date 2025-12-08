#pragma once

#include "origo/core/Layer.h"

#include "EditorContext.h"
#include "origo/core/UUID.h"

namespace OrigoEditor {

class SceneLayer : public Origo::Layer {
public:
	SceneLayer(EditorContext& ctx);

	void OnAttach() override;
	void OnUpdate(double dt) override;
	void OnEvent(Origo::Event& e) override;

private:
	void SpawnTestGrid();

private:
	static constexpr int GRID_SIZE { 20 };
	EditorContext& m_Context;

	Origo::UUID m_Shader { Origo::UUID::Bad() };
	Origo::UUID m_Texture { Origo::UUID::Bad() };
};

}
