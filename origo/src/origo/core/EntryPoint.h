#pragma once

#include "Application.h"

namespace Origo {
// to be defined by the client
extern Application* create_application();
}

int main(int argc, char** argv) {
	auto app = Origo::create_application();
	app->run();
	app->on_shutdown();
	delete app;
}
