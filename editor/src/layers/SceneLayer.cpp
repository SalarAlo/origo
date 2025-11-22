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

	static int counter { 0 };
	if (counter++ % 100 == 0) {
		ORG_INFO("{} FPS", 1.0 / dt);
	}

	for (auto t : m_Context.Scene.GetAllComponentsOfType<Transform>()) {
		auto normalized { glm::normalize(t->GetPosition()) };
		auto off = glm::vec3 { t->GetPosition().x, glm::sin(time + t->GetPosition().x + t->GetPosition().z), t->GetPosition().z };

		t->SetPosition(off);
	}
}

void SceneLayer::SpawnTestGrid() {
	auto cubeMesh = AssetManager::CreateAsset<Mesh>("Cube", PrimitiveShape::Cube);

	auto materialId {
		AssetManager::CreateAsset<Material>("Cube_Material", m_Shader, m_Texture)
	};

	auto material { AssetManager::GetAssetAs<Material>(materialId) };

	for (int i = 0; i < GRID_SIZE; ++i) {
		for (int j = 0; j < GRID_SIZE; ++j) {
			auto entity = m_Context.Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
			auto transform = m_Context.Scene.AddComponent<Transform>(entity);

			transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });
			transform->SetScale(glm::vec3 { 0.7f });

			m_Context.Scene.AddComponent<MeshRenderer>(entity, materialId, cubeMesh);
		}
	}
}
}
