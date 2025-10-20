#pragma once

#include "Application.h"
#include "Init.h"

namespace Origo {
// to be defined by the client
extern Application* CreateApplication();
}

int main(int argc, char** argv) {
	Origo::Init();

	auto app = Origo::CreateApplication();
	app->Run();
	app->OnShutdown();
	delete app;
}
