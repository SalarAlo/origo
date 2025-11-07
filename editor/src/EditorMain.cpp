#include "layer/SceneLayer.h"
#include "origo/assets/AssetManager.h"
#include "origo/core/EntryPoint.h"
#include "origo/core/Application.h"
#include "origo/renderer/FrameBuffer.h"

#include "layer/EditorCameraLayer.h"
#include "layer/EditorUILayer.h"
#include "EditorContext.h"
#include "origo/scene/Scene.h"

namespace OrigoEditor {

using namespace Origo;

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationSettings& settings)
	    : Application(settings)
	    , m_Buffer(1600, 900)
	    , m_Context(m_Scene, m_Buffer, m_Window.GetNativeWindow()) {
		PushLayer(new EditorCameraLayer(m_Scene.GetMainCamera().get()));
		PushLayer(new EditorUILayer(m_Context));
		PushLayer(new SceneLayer(m_Context));
	}

private:
	FrameBuffer m_Buffer;
	EditorContext m_Context;
};

}

namespace Origo {
Application* CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./",
		.WindowSettings = { .Width = 1800, .Height = 1000, .Title = "ITSAFLOATER" }
	};
	return new OrigoEditor::EditorApplication(settings);
}
}
