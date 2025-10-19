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
    // C++
    class GameApplication : public Origo::Application {
    public:
        GameApplication(const Origo::ApplicationSettings& s)
            : Origo::Application(s), m_Camera(m_Scene.GetCamera()) {}

        void Awake() override {
            m_Camera.SetSpeed(100);
            auto tex = Origo::TextureCache::Load("rowlett.jpg");
            m_Shader = Origo::ShaderLibrary::Load("normal");
            m_Material = Origo::MaterialLibrary::Create(m_Shader, tex);
            SpawnTestGrid();
        }

        void SpawnTestGrid() {
            auto model = Origo::ModelLibrary::Create("pikachu.glb");
            for (int i = 0; i < GRID_SIZE; ++i)
                for (int j = 0; j < GRID_SIZE; ++j)
                    for (const auto& mesh : model) {
                        auto entity = m_Scene.CreateEntity("Entity_" + std::to_string(i * GRID_SIZE + j));
                        auto t = m_Scene.m_ComponentManager.AddComponent<Origo::Transform>(entity);
                        t->SetPosition({ i * 40, j * 40, 11 });
                        t->SetScale({ 1 });
                        m_Scene.m_ComponentManager.AddComponent<Origo::MeshRenderer>(entity, m_Material, mesh);
                    }
        }

        void HandleEvent(Origo::Event& e) override { m_Camera.OnEvent(e); }

        void Update(double dt) override {
            glm::vec3 dir(0.0f);
            ORG_INFO("FPS: {}", 1.0 / dt);

            if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_W)) dir += m_Camera.GetForward();
            if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_S)) dir -= m_Camera.GetForward();
            if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_D)) dir += m_Camera.GetRight();
            if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_A)) dir -= m_Camera.GetRight();

            if (glm::length(dir) > 0.0f)
                m_Camera.Move(glm::normalize(dir) * static_cast<float>(dt));
        }

        void OnShutdown() override {
            Origo::SceneSerialization::Serialize("scene.json", m_Scene);
        }

    private:
        static constexpr int GRID_SIZE = 10;
        Origo::Camera& m_Camera;
        Origo::Ref<Origo::Shader> m_Shader;
        Origo::Ref<Origo::Material> m_Material;
    };
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
