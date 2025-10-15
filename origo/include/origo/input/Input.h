#pragma once

#include "origo/core/ScreenWindow.h"
#include "origo/events/KeyEvent.h"

namespace Origo::Input {
bool IsKeyPressed(KeyboardKey k);
void SetContext(Ref<ScreenWindow> win);
}
