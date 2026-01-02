#include "layer/RenderLayer.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/CubemapTexture.h"
#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"
#include "origo/assets/SkyboxMaterial.h"
#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"
#include "state/EditorRuntimeState.h"

using namespace Origo;

namespace OrigoEditor {
void RenderLayer::OnAttach() {
	CubemapDescription descr {};
	descr.Right = "./resources/textures/skybox/Right.png";
	descr.Left = "./resources/textures/skybox/Left.png";
	descr.Top = "./resources/textures/skybox/Top.png";
	descr.Bottom = "./resources/textures/skybox/Bottom.png";
	descr.Front = "./resources/textures/skybox/Front.png";
	descr.Back = "./resources/textures/skybox/Back.png";

	auto cubemapHandle { AssetFactory::CreateAsset<CubemapTexture>("skybox", descr) };
	auto cubemap { AssetManagerFast::GetInstance().Get<CubemapTexture>(cubemapHandle) };
	cubemap->Load();

	auto skyboxShaderHandle { AssetFactory::CreateAsset<Shader>("skybox") };
	auto skyboxShader { AssetManagerFast::GetInstance().Get<Shader>(skyboxShaderHandle) };
	skyboxShader->SetSource(MakeScope<ShaderSourceFile>("./resources/shaders/skybox.glsl"));

	m_SkyboxMaterial = AssetFactory::CreateAsset<SkyboxMaterial>("skyboxmaterial", skyboxShaderHandle, cubemapHandle);

	m_RenderContext.SetSkyboxMaterial(m_SkyboxMaterial);
}

void RenderLayer::OnUpdate(double dt) {
	const bool editingView = (m_Context.ViewMode == EditorViewMode::Editor);

	m_RenderContext.SetView(m_Context.ViewportController.GetActiveRenderView());

	m_RenderContext.BeginFrame();

	auto scenePtr = m_Context.ActiveScene;

	SystemScheduler::Get().RunPhase(GamePhase::RenderGeometry, scenePtr, m_RenderContext);

	if (editingView) {
		SystemScheduler::Get().RunPhase(GamePhase::RenderEditor, scenePtr, m_RenderContext);
	}

	m_RenderContext.Flush();

	m_RenderContext.EndFrame();
}

}
