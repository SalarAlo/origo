# Origo

A Linux-first 3D engine written in C++ and GLSL.  
Focused on clarity, control, and correctness.

<img src="resources/textures/origo_logo.png" alt="Origo Logo" width="400"/>

---

## Overview

Origo is a small, self-contained 3D engine built for Linux.  
No portability layers or cross-platform abstractions.  
Intended for experimentation, rendering research, and low-level engine work.

---

## Requirements

| Requirement   | Notes                         |
|--------------|-------------------------------|
| Linux        | Developed and tested on Fedora|
| C++ Compiler | C++20 or newer                |
| OpenGL       | Proper driver support required|
| CMake        | Build system                  |
| Optional     | stb_image, glm, etc.          |

---

## Project Layout

| Path              | Description                       |
|-------------------|-----------------------------------|
| `origo/`          | Engine source                     |
| `game/`           | Example application               |
| `editor/`         | WIP editor                        |
| `resources/`      | Shaders, textures, models         |
| `vendor/`         | Third-party deps                  |
| `CMakeLists.txt`  | Root build config                 |
| `pch.*`           | Precompiled headers               |
| `run.sh`          | Run helper                        |
| `generate.sh`     | Build helper                      |
| `overview.sh`     | Repo overview helper              |

---

## Example

A minimal client using the engine:

```cpp
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

## Building

    git clone https://github.com/<user>/Origo.git
    cd Origo
    mkdir build 
    ./generate.sh
  
## Run the example (from root if your on linux):
    
    ./run 

---

## Status

Under active development. APIs and layout may change.
