/*

int main() {
        Origo::ScreenWindowSettings settings { .Width = 1900, .Height = 1060, .Title = "ITSAFLOATER" };
        Origo::ScreenWindow window { settings };

        Origo::Camera camera { window.GetAspectResolution(), { 0, 0, -3 } };

        Origo::EventSystem eventSystem { window };

        Origo::VertexLayout layout {};
        layout.AddAttribute<float>(3);
        layout.AddAttribute<float>(3);
        layout.AddAttribute<float>(2);

        // link time error
        // layout.AddAttribute<bool>(3);

        Origo::Mesh CubeMesh {
                .Vertices = Origo::CUBE_VERTICES,
                .Indices = Origo::CUBE_INDICES,
                .Layout = layout
        };

        Origo::Renderable cube { CubeMesh };
        auto pikachu { Origo::ModelLoader::LoadModel("pikachu.glb", layout) };
        std::vector<Origo::Renderable> renderables {};
        for (const auto& mesh : pikachu) {
                renderables.emplace_back(mesh);
        }

        Origo::Shader shader { "normal" };

        Origo::TextureAtlas atlas {};
        int rowlettHandle { atlas.AddImageFromFile("rowlett.jpg") };
        atlas.Finalize();

        Origo::RenderTarget target(540, 540); // render at 640×360 internally

        int frames {};
        while (!window.ShouldClose()) {
                target.Bind();
                glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                atlas.Bind(0);

                shader.UseProgram();

                shader.SetUniformInt("u_Atlas", 0);
                // atlas.BindTexture(rowlettHandle, shader);

                shader.SetUniformMat4("u_ProjectionMatrix", camera.GetProjection());
                shader.SetUniformMat4("u_ViewMatrix", camera.GetView());

                shader.SetUniformVec3("u_LightPos", camera.GetPosition());
                shader.SetUniformVec3("u_ViewPos", camera.GetPosition());

                auto model { glm::mat4 { 1.0f } };
                model = glm::translate(model, { 0.0f, -4.0f, 4.0f });
                model = glm::rotate(model, glm::radians(180.0f), { 0.0f, 1, 0.0f });
                model = glm::scale(model, { 0.05f, 0.05f, 0.05f });
                shader.SetUniformMat4("u_ModelMatrix", model);

                for (auto& renderable : renderables) {
                        renderable.Render(shader);
                }

                target.DrawToScreen(window.GetWidth(), window.GetHeight());

                window.SwapBuffers();
                eventSystem.CheckEvents();
                frames++;
        }

        return 0;
}
*/
