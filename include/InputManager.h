#include "engine/Camera.h"
#include "engine/EventSystem.h"

class InputManager {
public:
	InputManager(Camera& camera, EventSystem& eventSystem);

private:
	Camera& m_Camera;
	EventSystem& m_EventSystem;
};
