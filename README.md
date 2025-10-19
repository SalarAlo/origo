<div align="center">

<h1>Origo</h1>

<p>A Linux-first 3D engine written in C++ and GLSL.<br>
Focused on clarity, control, and correctness.</p>

<img src="resources/textures/origo_logo.png" alt="Origo Logo" width="400"/>

<hr style="width:50%;margin:2rem auto;">

<h2>Overview</h2>

<p>
Origo is a small, self-contained 3D engine built for Linux.<br>
No portability layers or cross-platform abstractions.<br>
Intended for experimentation, rendering research, and low-level engine work.
</p>

<hr style="width:50%;margin:2rem auto;">

<h2>Requirements</h2>

<table>
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

<hr style="width:50%;margin:2rem auto;">

<h2>Project Layout</h2>

<table>
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

<hr style="width:50%;margin:2rem auto;">

<h2>Example</h2>

<p>A minimal client using the engine:</p>

<div style="display:inline-block; text-align:left; max-width:100%; overflow:auto;">
<pre><code class="language-cpp">class GameApplication : public Origo::Application {
public:
    GameApplication(const Origo::ApplicationSettings&amp; s)
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
        for (int i = 0; i &lt; GRID_SIZE; ++i)
            for (int j = 0; j &lt; GRID_SIZE; ++j)
                for (const auto&amp; mesh : model) {
                    auto entity = m_Scene.CreateEntity("Entity_" + std::to_string(i * GRID_SIZE + j));
                    auto t = m_Scene.m_ComponentManager.AddComponent&lt;Origo::Transform&gt;(entity);
                    t-&gt;SetPosition({ i * 40, j * 40, 11 });
                    t-&gt;SetScale({ 1 });
                    m_Scene.m_ComponentManager.AddComponent&lt;Origo::MeshRenderer&gt;(entity, m_Material, mesh);
                }
    }

    void HandleEvent(Origo::Event&amp; e) override { m_Camera.OnEvent(e); }

    void Update(double dt) override {
        glm::vec3 dir(0.0f);
        ORG_INFO("FPS: {}", 1.0 / dt);

        if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_W)) dir += m_Camera.GetForward();
        if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_S)) dir -= m_Camera.GetForward();
        if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_D)) dir += m_Camera.GetRight();
        if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_A)) dir -= m_Camera.GetRight();

        if (glm::length(dir) &gt; 0.0f)
            m_Camera.Move(glm::normalize(dir) * static_cast&lt;float&gt;(dt));
    }

    void OnShutdown() override {
        Origo::SceneSerialization::Serialize("scene.json", m_Scene);
    }

private:
    static constexpr int GRID_SIZE = 10;
    Origo::Camera&amp; m_Camera;
    Origo::Ref&lt;Origo::Shader&gt; m_Shader;
    Origo::Ref&lt;Origo::Material&gt; m_Material;
};</code></pre>
</div>

<hr style="width:50%;margin:2rem auto;">

<h2>Building</h2>

<div style="display:inline-block; text-align:left; max-width:100%; overflow:auto;">
<pre><code>git clone https://github.com/&lt;user&gt;/Origo.git
cd Origo
mkdir build 
./generate.sh</code></pre>
</div>

<p><strong>Run the example (from repo root on Linux):</strong></p>

<div style="display:inline-block; text-align:left; max-width:100%; overflow:auto;">
<pre><code>./run.sh</code></pre>
</div>

<hr style="width:50%;margin:2rem auto;">

<h2>Status</h2>

<p>Under active development. APIs and layout may change.</p>

</div>
