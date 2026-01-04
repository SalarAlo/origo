#include "layer/SceneLayer.h"

#include <random>

#include "components/FallComponent.h"
#include "components/EditorOutline.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/Material.h"
#include "origo/assets/ShaderSource.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/TextureSource.h"
#include "origo/assets/PrimitiveShape.h"
#include "origo/assets/Mesh.h"

#include "origo/core/Time.h"

#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/VertexLayoutRegistry.h"
#include "origo/renderer/VertexAttribute.h"

#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Transform.h"

using namespace Origo;

namespace OrigoEditor {

SceneLayer::SceneLayer(EditorContext& ctx)
    : m_Context(ctx) { }

void SceneLayer::OnAttach() {
	m_Texture = AssetFactory::CreateAsset<Texture2D>("Grid_Texture");
	AssetManagerFast::GetInstance()
	    .Get<Texture2D>(m_Texture)
	    ->SetSource(MakeScope<TextureSourceFile>("resources/textures/dirt.jpg"));

	m_Shader = AssetFactory::CreateAsset<Shader>("Grid_Shader");
	AssetManagerFast::GetInstance()
	    .Get<Shader>(m_Shader)
	    ->SetSource(MakeScope<ShaderSourceFile>("resources/shaders/normal.glsl"));

	VertexLayout layout {};
	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Position);
	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Normal);
	layout.AddAttribute<float>(2, false, VertexAttributeSemantic::TexCoord);

	m_VertexLayoutID = VertexLayoutRegistry::Register(layout);
	m_VertexStride = layout.GetStride();

	m_HeapID = GeometryHeapRegistry::CreateHeap(
	    m_VertexLayoutID,
	    GL_STATIC_DRAW,
	    120'000,
	    120'000);

	CreateAssets();

	SpawnGrid(5);
}

void SceneLayer::OnUpdate(double) {
	auto shader = AssetManagerFast::GetInstance().Get<Shader>(m_Shader);

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
		MeshData data = GetDataFromShape(PrimitiveShape::Cube);

		auto range = heap->Allocate(
		    data.Vertices.data(),
		    data.Vertices.size() * sizeof(float),
		    m_VertexStride,
		    data.Indices.data(),
		    data.Indices.size());

		m_CubeMesh = AssetFactory::CreateAsset<Mesh>(
		    "Grid_Cube_Mesh",
		    m_VertexLayoutID,
		    m_HeapID,
		    range);
	}

	{
		MeshData data = GetDataFromShape(PrimitiveShape::Sphere);

		auto range = heap->Allocate(
		    data.Vertices.data(),
		    data.Vertices.size() * sizeof(float),
		    m_VertexStride,
		    data.Indices.data(),
		    data.Indices.size());

		AssetFactory::CreateAsset<Mesh>(
		    "Grid_Sphere_Mesh",
		    m_VertexLayoutID,
		    m_HeapID,
		    range);
	}

	m_Material = AssetFactory::CreateAsset<Material2D>(
	    "Grid_Material",
	    m_Shader,
	    m_Texture);
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
			    m_Material,
			    m_CubeMesh);

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
