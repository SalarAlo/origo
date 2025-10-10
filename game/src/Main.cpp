#include <origo/Origo.h>

class GameApplication : public Origo::Application {
public:
	GameApplication(const Origo::ApplicationSettings& applicationSettings)
	    : Origo::Application(applicationSettings) {
	}

	void OnEvent(const Origo::Event& event) override {
	}

private:
};

Origo::Application* Origo::CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./game",
		// i use i3. in my config apps with "ITSAFLOATER" title are floating. thats why i call it like that
		.WindowSettings = { .Width = 1000, .Height = 1000, .Title = "ITSAFLOATER" }
	};

	return new GameApplication(settings);
}
