<h1 align="center">Origo</h1>

<p align="center">
A Linux-first 3D engine written in C++ and GLSL.<br>
Focused on clarity, control, and correctness.
</p>

<p align="center">
  <img src="resources/textures/origo_logo.png" alt="Origo Logo" width="400"/>
</p>

---

<h2 align="center">Overview</h2>

<p align="center">
Origo is a small, self-contained 3D engine built for Linux.  
No portability layers or cross-platform abstractions.  
Intended for experimentation, rendering research, and low-level engine work.
</p>

---
<h2 align="center">Requirements</h2>

<div align="center">
  <table style="margin:auto;">
    <thead>
      <tr>
        <th>Requirement</th>
        <th>Notes</th>
      </tr>
    </thead>
    <tbody>
      <tr><td>Linux</td><td>Developed and tested on Fedora</td></tr>
      <tr><td>C++ Compiler</td><td>C++20 or newer</td></tr>
      <tr><td>OpenGL</td><td>Proper driver support required</td></tr>
      <tr><td>CMake</td><td>Build system</td></tr>
      <tr><td>Optional</td><td>stb_image, glm, etc.</td></tr>
    </tbody>
  </table>
</div>

<hr/>

<h2 align="center">Project Layout</h2>

<div align="center">
  <table style="margin:auto;">
    <thead>
      <tr>
        <th>Path</th>
        <th>Description</th>
      </tr>
    </thead>
    <tbody>
      <tr><td><code>origo/</code></td><td>Engine source</td></tr>
      <tr><td><code>game/</code></td><td>Example application</td></tr>
      <tr><td><code>editor/</code></td><td>WIP editor</td></tr>
      <tr><td><code>resources/</code></td><td>Shaders, textures, models</td></tr>
      <tr><td><code>vendor/</code></td><td>Third-party deps</td></tr>
      <tr><td><code>CMakeLists.txt</code></td><td>Root build config</td></tr>
      <tr><td><code>pch.*</code></td><td>Precompiled headers</td></tr>
      <tr><td><code>run.sh</code></td><td>Run helper</td></tr>
      <tr><td><code>generate.sh</code></td><td>Build helper</td></tr>
      <tr><td><code>overview.sh</code></td><td>Repo overview helper</td></tr>
    </tbody>
  </table>
</div>

---

<h2 align="center">Example</h2>

<p align="center">
A minimal client using the engine:
</p>

```cpp
// headers...

class GameApplication : public Origo::Application {
public:
	GameApplication(const Origo::ApplicationSettings& applicationSettings)
	    : Origo::Application(applicationSettings)
	    , m_Camera(m_Scene.GetMainCamera()) {
	}

	void OnAwake() override {
		m_Camera->SetSpeed(10);

		auto logoTexture { Origo::AssetManager::CreateAsset<Origo::Texture>("Rowlett", "resources/textures/rowlett.jpg") };

		m_Shader = Origo::AssetManager::CreateAsset<Origo::Shader>("Normal Shader", "normal");
		m_Material = Origo::AssetManager::CreateAsset<Origo::Material>("Normal Material", m_Shader, logoTexture);

		SpawnTestGrid();
	}

	void SpawnTestGrid() {
		auto cubeMesh = Origo::AssetManager::CreateAsset<Origo::Mesh>("Cube", Origo::PrimitiveShape::Cube);

		for (int i {}; i < GRID_SIZE; i++) {
			for (int j {}; j < GRID_SIZE; j++) {
				auto entity = m_Scene.CreateEntity("Cube_" + std::to_string(i * GRID_SIZE + j));
				auto transform = m_Scene.AddComponent<Origo::Transform>(entity);
				transform->SetPosition(glm::vec3 { i * 2, 0, j * 2 });
				m_Scene.AddComponent<Origo::MeshRenderer>(entity, m_Material, cubeMesh);
			}
		}
	}

	void OnHandleEvent(Origo::Event& event) override {
		m_Camera->OnEvent(event);
		Origo::EventDispatcher evtDispatcher { event };
		evtDispatcher.Dispatch<Origo::KeyPressEvent>([&](Origo::KeyPressEvent& e) {
			if (e.GetKeyPressed() == Origo::KeyboardKey::KEY_ESCAPE) {
				Origo::Input::SetCursorMode(Origo::Input::CursorMode::Free);
				m_Camera->SetSensitivity(0);
			} else if (e.GetKeyPressed() == Origo::KeyboardKey::KEY_BACKSPACE) {
				Origo::Input::SetCursorMode(Origo::Input::CursorMode::Locked);
				m_Camera->SetSensitivity(0.1);
			}
		});
	}

	void OnUpdate(double dt) override {
		glm::vec3 direction(0.0f);
		constexpr float normalSpeed { 10 };
		constexpr float fastSpeed { normalSpeed * 5 };

		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_LEFT_SHIFT)) {
			m_Camera->SetSpeed(fastSpeed);
		} else {
			m_Camera->SetSpeed(normalSpeed);
		}

		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_W))
			direction += m_Camera->GetForward();
		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_S))
			direction -= m_Camera->GetForward();
		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_D))
			direction += m_Camera->GetRight();
		if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_A))
			direction -= m_Camera->GetRight();

		if (glm::length(direction) > 0.0f)
			m_Camera->Move(glm::normalize(direction) * static_cast<float>(dt));
	}

	void OnShutdown() override {
		Origo::JsonSerializer serializer { "./resources/scenes/some.json" };
		Origo::SceneSerialization::Serialize(m_Scene, serializer);
	}

	void OnImGuiRender() override {
	}

private:
	static constexpr int GRID_SIZE { 10 };

	Origo::Ref<Origo::Camera> m_Camera;
	Origo::Ref<Origo::Shader> m_Shader {};
	Origo::Ref<Origo::Material> m_Material {};
};

Origo::Application* Origo::CreateApplication() {
	ApplicationSettings settings {
		.WorkingDirectory = "./game",
		.WindowSettings = { .Width = 1800, .Height = 1000, .Title = "ITSAFLOATER" }
	};
	return new GameApplication(settings);
}

```
---

<h2 align="center">Building</h2>

<div align="">
  <pre><code>git clone https://github.com/&lt;user&gt;/Origo.git
cd Origo
mkdir build
./generate.sh</code></pre>
</div>

<p align="center"><strong>Run the example (from repo root on Linux):</strong></p>

<div align="">
  <pre><code>./run.sh</code></pre>
</div>

---

<h2 align="center">Status</h2>

<p align="center">
Under active development. APIs and layout may change.
</p>
