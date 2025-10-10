#pragma once
#include "origo/events/Event.h"
#include <string>

namespace Origo {
struct ScreenWindowSettings {
	int Width { 800 };
	int Height { 600 };
	std::string Title { "Origo Application" };
	EventCallbackFn EventCallback { nullptr };
};
}
