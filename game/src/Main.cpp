#include <origo/Origo.h>

class GameApplication : public Origo::Application {
public:
	GameApplication(const Origo::ApplicationSettings& applicationSettings)
	    : Origo::Application(applicationSettings) {
	}

private:
};

Origo::Application* Origo::CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./game",
		.WindowSettings = { .Width = 1000, .Height = 1000, .Title = "Game Application" }
	};
	return new GameApplication(settings);
}
