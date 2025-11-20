#include "layer/SceneLayer.h"

#include "origo/assets/Material.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Mesh.h"
#include "origo/assets/Texture.h"
#include "origo/core/Time.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"

using namespace Origo;
static std::hash<RID> HashEntity {};

namespace OrigoEditor {
SceneLayer::SceneLayer(EditorContext& ctx)
    : m_Context(ctx) { }

void SceneLayer::OnAttach() {
	m_Texture = AssetManager::CreateAsset<Texture>("Rowlett", "resources/textures/dirt.jpg");
	m_Shader = AssetManager::CreateAsset<Shader>("Normal Shader", "normal");

	SpawnTestGrid();
}

void SceneLayer::OnEvent(Event& e) {
}

void SceneLayer::OnUpdate(double dt) {
	auto shader { AssetManager::GetAssetAs<Shader>(m_Shader) };
	shader->UseProgram();

	float time = Time::GetTimeSinceStart().count();
	shader->SetUniform("u_Time", time);

	int selectedEntityId { -1 };
	if (m_Context.SelectedEntity != nullptr) {
		selectedEntityId = static_cast<int>(HashEntity(m_Context.SelectedEntity->GetId()));
	}

	shader->SetUniform("u_SelectedEntityId", selectedEntityId);
	shader->SetUniform("u_ScreenSize", glm::vec2(m_Context.Buffer.GetWidth(), m_Context.Buffer.GetHeight()));
}

void SceneLayer::SpawnTestGrid() {
	auto cubeMesh = AssetManager::CreateAsset<Mesh>("Cube", PrimitiveShape::Sphere);

	for (int i = 0; i < GRID_SIZE; ++i) {
		for (int j = 0; j < GRID_SIZE; ++j) {
			auto entity = m_Context.Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
			auto transform = m_Context.Scene.AddComponent<Transform>(entity);

			transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });

			auto materialId {
				AssetManager::CreateAsset<Material>(
				    "Cube_Material_" + std::to_string(i * GRID_SIZE + j),
				    m_Shader,
				    m_Texture)
			};

			auto material { AssetManager::GetAssetAs<Material>(materialId) };

			material->GetUniformList().AddUniform(
			    "u_CurrentEntityId", static_cast<int>(HashEntity(entity->GetId())));

			m_Context.Scene.AddComponent<MeshRenderer>(entity, materialId, cubeMesh);
		}
	}
}
}
