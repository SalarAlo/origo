#include "layers/SceneLayer.h"

#include <random>

#include "components/FallComponent.h"
#include "components/EditorOutline.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/Material.h"
#include "origo/assets/PrimitiveShape.h"
#include "origo/assets/Mesh.h"

#include "origo/assets/PrimitiveShapeCache.h"
#include "origo/core/Time.h"

#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/VertexLayout.h"
#include "origo/renderer/VertexLayoutRegistry.h"

#include "origo/scene/CameraComponent.h"
#include "origo/scene/Light.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"

using namespace Origo;

namespace OrigoEditor {

SceneLayer::SceneLayer(EditorContext& ctx)
    : m_Context(ctx) {
}

void SceneLayer::OnAttach() {
	m_VertexLayoutID = VertexLayout::GetStaticMeshLayout();
	m_VertexStride = VertexLayoutRegistry::Get(m_VertexLayoutID)->GetStride();

	m_HeapID = GeometryHeapRegistry::GetOrCreateStaticMeshHeap(m_VertexLayoutID);

	auto scene { m_Context.ActiveScene };
	auto cam { scene->CreateEntity("Main Camera") };

	auto cameraTransformComponent { scene->GetNativeComponent<Transform>(cam) };
	auto cameraComponent { scene->AddNativeComponent<Origo::CameraComponent>(cam) };
	cameraComponent->IsPrimary = true;
	cameraTransformComponent->SetPosition({ -3.5, 4, 5 });
	cameraTransformComponent->SetRotation({ -33, -33, 0 });

	auto lightEntity { scene->CreateEntity("Light") };
	auto lightTransformComponent { scene->GetNativeComponent<Transform>(lightEntity) };
	auto lightComponent { scene->AddNativeComponent<Origo::Light>(lightEntity) };
	lightComponent->SetShaderTarget(Shader::DefaultShader());
	lightTransformComponent->SetPosition({ 0, 4.0, 0.0 });

	CreateAssets();

	SpawnGrid(4);
}

void SceneLayer::OnUpdate(double) {
	auto shader = AssetManager::GetInstance().Get<Shader>(Shader::DefaultShader());

	shader->UseProgram();
	shader->SetUniform("u_Time",
	    static_cast<float>(Time::GetTimeSinceStart().count()));
	shader->SetUniform(
	    "u_ScreenSize",
	    glm::vec2(
	        m_Context.RenderBuffer.GetWidth(),
	        m_Context.RenderBuffer.GetHeight()));
}

void SceneLayer::CreateAssets() {
	auto heap = GeometryHeapRegistry::GetHeap(m_HeapID);

	{
		MeshData data = GetDataFromShape(PrimitiveShape::Sphere);

		auto range = heap->Allocate(
		    data.Vertices.data(),
		    data.Vertices.size() * sizeof(float),
		    m_VertexStride,
		    data.Indices.data(),
		    data.Indices.size());

		AssetFactory::CreateRuntimeAsset<Mesh>(
		    "Grid_Sphere_Mesh",
		    m_VertexLayoutID,
		    m_HeapID,
		    range);
	}
}

void SceneLayer::SpawnGrid(int gridSize, float spacing) {
	std::mt19937 rng { std::random_device {}() };
	std::uniform_int_distribution<int> chance { 0, 3 };
	std::uniform_real_distribution<float> speed { 1.0f, 10.0f };

	const float half = (gridSize - 1) * spacing * 0.5f;

	int id = 0;

	for (int x = 0; x < gridSize; ++x) {
		for (int z = 0; z < gridSize; ++z) {
			glm::vec3 pos {
				x * spacing - half,
				0.0f,
				z * spacing - half
			};

			auto entity = m_Context.EditorScene->CreateEntity(
			    "GridCube_" + std::to_string(id++));

			auto transform = m_Context.EditorScene->AddNativeComponent<Transform>(entity);
			transform->SetPosition(pos);

			m_Context.EditorScene->AddNativeComponent<MeshRenderer>(
			    entity,
			    Material2D::DefaultMaterial2D(),
			    PrimitiveShapeCache::GetInstance().GetCubeMesh());

			m_Context.EditorScene->AddNativeComponent<EditorOutline>(entity);

			if (!chance(rng)) {
				m_Context.EditorScene->AddNativeComponent<FallComponent>(
				    entity,
				    speed(rng));
			}
		}
	}
}

}
