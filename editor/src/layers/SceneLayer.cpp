#include "layers/SceneLayer.h"

#include "origo/assets/PrimitiveShapeCache.h"
#include "origo/core/Time.h"

#include "origo/assets/Shader.h"

#include "origo/scene/CameraComponent.h"
#include "origo/scene/DirectionalLight.h"
#include "origo/scene/Transform.h"
#include "origo/assets/Material.h"

using namespace Origo;

namespace OrigoEditor {

SceneLayer::SceneLayer(EditorContext& ctx)
    : m_Context(ctx) {
}

void SceneLayer::OnAttach() {
	auto scene { m_Context.ActiveScene };
	auto cam { scene->CreateEntity("Main Camera") };

	auto cameraTransformComponent { scene->GetNativeComponent<Transform>(cam) };
	auto cameraComponent { scene->AddNativeComponent<Origo::CameraComponent>(cam) };
	cameraComponent->IsPrimary = true;
	cameraTransformComponent->SetPosition({ -3.5, 4, 5 });
	cameraTransformComponent->SetRotation({ -33, -33, 0 });

	auto lightEntity { scene->CreateEntity("Light") };
	auto lightTransformComponent { scene->GetNativeComponent<Transform>(lightEntity) };
	auto lightComponent { scene->AddNativeComponent<Origo::DirectionalLight>(lightEntity) };
	lightComponent->SetShaderTarget(Shader::DefaultShader());
	lightTransformComponent->SetPosition({ 4.0, 4.0, 3.0 });
	lightTransformComponent->LookAt({ 0, 0, 0 });

	PrimitiveShapeCache::GetInstance();
	Material2D::DefaultMaterial2D();
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

}
