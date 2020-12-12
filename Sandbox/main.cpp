#include <spark.h>
#include <spark/core/entry_point.h>

class SandboxLayer : public Spark::Layer2D
{
  public:
    SandboxLayer()
        : Spark::Layer2D("Sandbox layer")
        , counter(0)
    {
        const Spark::Texture &texture = Spark::ResourceManager::loadTexture("sandboxTexture", "textures/texture.jpg");
        Spark::ResourceManager::loadTexture("sandboxTexture2", "textures/test.jpg");
        m_quads.push_back(std::move(Spark::createQuad(glm::vec2(0, 0), texture, glm::vec2(1))));
        addDrawable(m_quads.back());
    }

    virtual void OnAttach()
    {
        SPARK_INFO("Attached {0}", getName());
        Layer2D::OnAttach();
    }

    virtual void OnDetach()
    {
        Layer2D::OnDetach();
        SPARK_INFO("Detached {0}", getName());
    }

    virtual void OnUpdate(Spark::Time &diffTime)
    {
        Layer2D::OnUpdate(diffTime);
    }

    bool handleKeyPressed(Spark::KeyPressedEvent &e)
    {
        switch (e.GetKeyCode())
        {
        case Spark::KeyCode::Left:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->move({-0.05, 0});
            return true;
        case Spark::KeyCode::Right:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->move({0.05, 0});
            return true;
        case Spark::KeyCode::Down:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->move({0, 0.05});
            return true;
        case Spark::KeyCode::Up:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->move({0, -0.05});
            return true;
        case Spark::KeyCode::N:
            counter++;
            m_quads.push_back(std::move(Spark::createQuad(glm::vec2(-0.5 + counter / 10.0, -0.5),
                                                          *Spark::ResourceManager::getTexture("sandboxTexture2"))));
            addDrawable(m_quads.back());
            return true;
        case Spark::KeyCode::M:
            removeDrawable(m_quads.back().get());
            m_quads.pop_back();
            return true;
        case Spark::KeyCode::Z:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->scale(glm::vec2(0.95f));
            return true;
        case Spark::KeyCode::X:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->scale(glm::vec2(1.05f));
            return true;
        case Spark::KeyCode::C:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->scale(glm::vec2(0.95f, 1));
            return true;
        case Spark::KeyCode::V:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->scale(glm::vec2(1.05f, 1));
            return true;
        case Spark::KeyCode::F:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->scale(glm::vec2(1, 0.95f));
            return true;
        case Spark::KeyCode::G:
            reinterpret_cast<Spark::Quad *>(m_quads.front().get())->scale(glm::vec2(1, 1.05f));
            return true;
        default:
            return false;
        }
    }

    virtual void OnEvent(Spark::Event &e)
    {
        Layer2D::OnEvent(e);
        Spark::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(SandboxLayer::handleKeyPressed));
    }

  private:
    int counter;
    std::vector<std::shared_ptr<Spark::Drawable>> m_quads;
};

class Sandbox3DLayer : public Spark::Layer3D
{
  public:
    Sandbox3DLayer()
        : m_camera({10.0f, 0.0f, 0.0f})
        , Spark::Layer3D("Sandbox layer", m_camera)
        , m_focused(false)
        , m_addingBox(false)
        , m_removingBox(false)
        , m_setDirLight(false)
        , m_setSpotLight(false)
        , m_addingLightBox(false)
        , m_removingLightBox(false)
        , m_nextCords{0, 0, 0}
        , m_nextLightsCords{0, 0, 0}
        , m_dirLightColor{1.0f, 1.0f, 1.0f}
        , m_dirLightDirection{-0.2f, -1.0f, -0.3f}
        , m_spotLightColor{1.0f, 1.0f, 1.0f}
        , m_pointLightColor{1.0f, 1.0f, 1.0f}
        , m_removeBoxIndex(0)
        , m_previousRemoveBoxIndex(0)
        , m_removeLightIndex(0)
    {
        const Spark::Texture &texture = Spark::ResourceManager::loadTexture("cubeTexutre", "textures/container2.png");
        const Spark::Texture &specularTexture =
            Spark::ResourceManager::loadTexture("cubeTexutreSpecular", "textures/container2_specular.png");
        m_cubes.push_back(std::move(Spark::createCube({0, 0, 0}, texture, specularTexture)));
        addDrawable(m_cubes[0]);
        setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                    {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
        m_pointLights.push_back(Spark::createPointLight(
            {0, 0, -2.0f}, {0, 1, 0}, Spark::createCube({0, 0, -2.0f}, {0.3, 0.3f, 0.3f}, {0.3, 0.3f, 0.3f})));
        addPointLight(*(m_pointLights.back()));
        setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
    }

    virtual void OnAttach()
    {
        SPARK_INFO("Attached {0}", getName());
        Layer3D::OnAttach();
    }

    virtual void OnDetach()
    {
        Spark::Input::UnHideMouse();
        Layer3D::OnDetach();
        SPARK_INFO("Detached {0}", getName());
    }

    virtual void OnUpdate(Spark::Time &diffTime)
    {
        Layer3D::OnUpdate(diffTime);

        if (!m_focused)
        {
            return;
        }

        if (Spark::Input::IsKeyPressed(Spark::KeyCode::A))
        {
            m_camera.moveDirection(Spark::CameraDirection::LEFT, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::D))
        {
            m_camera.moveDirection(Spark::CameraDirection::RIGHT, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::W))
        {
            m_camera.moveDirection(Spark::CameraDirection::FORWARD, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::S))
        {
            m_camera.moveDirection(Spark::CameraDirection::BACKWARD, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::LeftShift))
        {
            m_camera.moveDirection(Spark::CameraDirection::UP, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::LeftControl))
        {
            m_camera.moveDirection(Spark::CameraDirection::DOWN, diffTime);
        }
    }

    virtual void OnEvent(Spark::Event &e)
    {
        Layer3D::OnEvent(e);
        Spark::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Spark::MouseMovedEvent>(SPARK_BIND_EVENT_FN(Sandbox3DLayer::handleMouseMoved));
        dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox3DLayer::handleKeyPressed));
        dispatcher.Dispatch<Spark::MouseButtonPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox3DLayer::handleMousePressed));
    }

    bool handleMouseMoved(Spark::MouseMovedEvent &e)
    {
        if (!m_focused)
        {
            return false;
        }

        m_camera.moveAngle(e.GetDiffX(), e.GetDiffY());
        return true;
    }

    bool handleKeyPressed(Spark::KeyPressedEvent &e)
    {
        ImGuiIO &io = ImGui::GetIO();
        switch (e.GetKeyCode())
        {
        case Spark::KeyCode::Escape:
            Spark::Input::UnHideMouse();
            m_focused = false;
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            return true;
        default:
            return false;
        }
    }

    bool handleMousePressed(Spark::MouseButtonPressedEvent &e)
    {
        ImGuiIO &io = ImGui::GetIO();
        switch (e.GetMouseButton())
        {
        case Spark::MouseCode::ButtonLeft:
            Spark::Input::HideMouse();
            m_focused = true;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
            return true;
        default:
            return false;
        }
    }

    void generateOverlay()
    {
        ImGui::SetNextWindowSize(ImVec2(350, 170 + 30.0f * m_pointLights.size()), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 360, 10), ImGuiCond_Once);
        ImGui::Begin("3d editor", NULL, ImGuiWindowFlags_NoResize);

        if (ImGui::Button("add box"))
        {
            SPARK_INFO("Adding box");
            m_addingBox = true;
        }

        if (m_addingBox)
        {
            ImGui::SetNextWindowSize(ImVec2(260, 35), ImGuiCond_Always);
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 195 / 2, ImGui::GetIO().DisplaySize.y / 2 - 35 / 2),
                ImGuiCond_Once);
            ImGui::Begin("Box adder", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

            ImGui::PushItemWidth(150);
            ImGui::InputFloat3("", m_nextCords);
            ImGui::PopItemWidth();
            ImGui::SameLine(165);
            if (ImGui::Button("add"))
            {
                const Spark::Texture *texture = Spark::ResourceManager::getTexture("cubeTexutre");
                const Spark::Texture *specularTexture = Spark::ResourceManager::getTexture("cubeTexutreSpecular");
                m_cubes.push_back(std::move(
                    Spark::createCube({m_nextCords[0], m_nextCords[1], m_nextCords[2]}, *texture, *specularTexture)));
                addDrawable(m_cubes.back());
                for (int i = 0; i < 3; i++)
                {
                    m_nextCords[i] = 0;
                }
                m_addingBox = false;
            }
            ImGui::SameLine(200);
            if (ImGui::Button("cancel"))
            {
                for (int i = 0; i < 3; i++)
                {
                    m_nextCords[i] = 0;
                }
                m_addingBox = false;
            }

            ImGui::End();
        }

        if (ImGui::Button("remove box"))
        {
            SPARK_INFO("Removing box");
            m_removingBox = true;
            if (m_cubes.size() > 0)
            {
                reinterpret_cast<Spark::Cube *>(m_cubes[0].get())->highlight();
            }
        }

        if (m_removingBox)
        {
            ImGui::SetNextWindowSize(ImVec2(220, 35), ImGuiCond_Always);
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 210 / 2, ImGui::GetIO().DisplaySize.y / 2 - 35 / 2),
                ImGuiCond_Once);
            ImGui::Begin("Box remover", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

            ImGui::PushItemWidth(75);
            if (ImGui::InputInt("", &m_removeBoxIndex))
            {
                if (m_removeBoxIndex >= 0 && m_removeBoxIndex < m_cubes.size())
                {
                    reinterpret_cast<Spark::Cube *>(m_cubes[m_previousRemoveBoxIndex].get())->unhighlight();
                    reinterpret_cast<Spark::Cube *>(m_cubes[m_removeBoxIndex].get())->highlight();
                    m_previousRemoveBoxIndex = m_removeBoxIndex;
                }
                else if (m_removeBoxIndex < 0)
                {
                    m_removeBoxIndex = 0;
                    reinterpret_cast<Spark::Cube *>(m_cubes[m_previousRemoveBoxIndex].get())->unhighlight();
                    reinterpret_cast<Spark::Cube *>(m_cubes[m_removeBoxIndex].get())->highlight();
                    m_previousRemoveBoxIndex = m_removeBoxIndex;
                }
                else
                {
                    m_removeBoxIndex = static_cast<int>(m_cubes.size()) - 1;
                    reinterpret_cast<Spark::Cube *>(m_cubes[m_previousRemoveBoxIndex].get())->unhighlight();
                    reinterpret_cast<Spark::Cube *>(m_cubes[m_removeBoxIndex].get())->highlight();
                    m_previousRemoveBoxIndex = m_removeBoxIndex;
                }
            }
            ImGui::PopItemWidth();
            ImGui::SameLine(90);
            if (ImGui::Button("remove"))
            {
                removeDrawable(m_cubes[m_removeBoxIndex].get());
                m_cubes.erase(m_cubes.begin() + m_removeBoxIndex);
                m_removeBoxIndex = 0;
                m_previousRemoveBoxIndex = 0;
                m_removingBox = false;
            }
            ImGui::SameLine(150);
            if (ImGui::Button("cancel"))
            {
                reinterpret_cast<Spark::Cube *>(m_cubes[m_removeBoxIndex].get())->unhighlight();
                m_removeBoxIndex = 0;
                m_previousRemoveBoxIndex = 0;
                m_removingBox = false;
            }

            ImGui::End();
        }

        if (ImGui::Button("set dir light"))
        {
            SPARK_INFO("Setting dir light");
            m_setDirLight = true;
        }
        ImGui::SameLine(130);
        if (ImGui::Button("dir light off"))
        {
            SPARK_INFO("Turning off dir light");
            setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]}, {0, 0, 0});
        }
        ImGui::SameLine(250);
        if (ImGui::Button("dir light on"))
        {
            SPARK_INFO("Turning on dir light");
            setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                        {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
        }

        if (m_setDirLight)
        {
            ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_Always);
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 210 / 2, ImGui::GetIO().DisplaySize.y / 2 - 35 / 2),
                ImGuiCond_Once);
            ImGui::Begin("Dir light setter", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

            ImGui::InputFloat3("direction", m_dirLightDirection);
            ImGui::InputFloat3("color", m_dirLightColor);
            if (ImGui::Button("set"))
            {
                setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                            {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
                m_setDirLight = false;
            }
            ImGui::SameLine(100);
            if (ImGui::Button("cancel"))
            {
                m_setDirLight = false;
            }

            ImGui::End();
        }

        if (ImGui::Button("set spot light"))
        {
            SPARK_INFO("Setting spot light");
            m_setSpotLight = true;
        }
        ImGui::SameLine(130);
        if (ImGui::Button("spot light off"))
        {
            SPARK_INFO("Turning off spot light");
            setSpotLight({0, 0, 0});
        }
        ImGui::SameLine(250);
        if (ImGui::Button("spot light on"))
        {
            SPARK_INFO("Turning on spot light");
            setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
        }

        if (m_setSpotLight)
        {
            ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_Always);
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 210 / 2, ImGui::GetIO().DisplaySize.y / 2 - 35 / 2),
                ImGuiCond_Once);
            ImGui::Begin("Spot light setter", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

            ImGui::InputFloat3("color", m_spotLightColor);
            if (ImGui::Button("set"))
            {
                setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
                m_setSpotLight = false;
            }
            ImGui::SameLine(100);
            if (ImGui::Button("cancel"))
            {
                m_setSpotLight = false;
            }

            ImGui::End();
        }

        if (ImGui::Button("add light box"))
        {
            SPARK_INFO("Adding light box");
            m_addingLightBox = true;
        }

        if (m_addingLightBox)
        {
            ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 195 / 2, ImGui::GetIO().DisplaySize.y / 2 - 35 / 2),
                ImGuiCond_Once);
            ImGui::Begin("Light box adder", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

            ImGui::InputFloat3("location", m_nextLightsCords);
            ImGui::InputFloat3("color", m_pointLightColor);
            if (ImGui::Button("add"))
            {
                m_pointLights.push_back(Spark::createPointLight(
                    {m_nextLightsCords[0], m_nextLightsCords[1], m_nextLightsCords[2]},
                    {m_pointLightColor[0], m_pointLightColor[1], m_pointLightColor[2]},
                    Spark::createCube({m_nextLightsCords[0], m_nextLightsCords[1], m_nextLightsCords[2]},
                                      {0.3f, 0.3f, 0.3f}, {0.3f, 0.3f, 0.3f})));
                addPointLight(*(m_pointLights.back()));
                for (int i = 0; i < 3; i++)
                {
                    m_nextLightsCords[i] = 0;
                    m_pointLightColor[i] = 1;
                }
                m_addingLightBox = false;
            }
            ImGui::SameLine(100);
            if (ImGui::Button("cancel"))
            {
                for (int i = 0; i < 3; i++)
                {
                    m_nextLightsCords[i] = 0;
                    m_pointLightColor[i] = 1;
                }
                m_addingLightBox = false;
            }

            ImGui::End();
        }

        if (ImGui::Button("remove light box"))
        {
            SPARK_INFO("Removing light box");
            m_removingLightBox = true;
            m_removeLightIndex = 0;
            m_previousRemoveLightIndex = 0;
            if (m_pointLights.size() > 0)
            {
                reinterpret_cast<Spark::Cube *>(m_pointLights[0].get()->drawable.get())->highlight();
            }
        }

        if (m_removingLightBox)
        {
            ImGui::SetNextWindowSize(ImVec2(220, 35), ImGuiCond_Always);
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 210 / 2, ImGui::GetIO().DisplaySize.y / 2 - 35 / 2),
                ImGuiCond_Once);
            ImGui::Begin("Light box remover", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

            ImGui::PushItemWidth(75);
            if (ImGui::InputInt("", &m_removeLightIndex))
            {
                if (m_removeLightIndex >= 0 && m_removeLightIndex < m_pointLights.size())
                {
                    reinterpret_cast<Spark::Cube *>(m_pointLights[m_previousRemoveLightIndex].get()->drawable.get())
                        ->unhighlight();
                    reinterpret_cast<Spark::Cube *>(m_pointLights[m_removeLightIndex].get()->drawable.get())
                        ->highlight();
                    m_previousRemoveLightIndex = m_removeLightIndex;
                }
                else if (m_removeLightIndex < 0)
                {
                    m_removeLightIndex = 0;
                    reinterpret_cast<Spark::Cube *>(m_pointLights[m_previousRemoveLightIndex].get()->drawable.get())
                        ->unhighlight();
                    reinterpret_cast<Spark::Cube *>(m_pointLights[m_removeLightIndex].get()->drawable.get())
                        ->highlight();
                    m_previousRemoveLightIndex = m_removeLightIndex;
                }
                else
                {
                    m_removeLightIndex = static_cast<int>(m_pointLights.size()) - 1;
                    reinterpret_cast<Spark::Cube *>(m_pointLights[m_previousRemoveLightIndex].get()->drawable.get())
                        ->unhighlight();
                    reinterpret_cast<Spark::Cube *>(m_pointLights[m_removeLightIndex].get()->drawable.get())
                        ->highlight();
                    m_previousRemoveLightIndex = m_removeLightIndex;
                }
            }
            ImGui::PopItemWidth();
            ImGui::SameLine(90);
            if (ImGui::Button("remove"))
            {
                removePointLight(*(m_pointLights[m_removeLightIndex].get()));
                m_pointLights.erase(m_pointLights.begin() + m_removeLightIndex);
                m_removeLightIndex = 0;
                m_previousRemoveLightIndex = 0;
                m_removingLightBox = false;
            }
            ImGui::SameLine(150);
            if (ImGui::Button("cancel"))
            {
                reinterpret_cast<Spark::Cube *>(m_pointLights[m_removeLightIndex].get()->drawable.get())->unhighlight();
                m_removeLightIndex = 0;
                m_previousRemoveLightIndex = 0;
                m_removingLightBox = false;
            }

            ImGui::End();
        }

        int index = 0;
        for (auto &pointLight : m_pointLights)
        {
            if (ImGui::Button(("Switch light cube " + std::to_string(index)).c_str()))
            {
                if (pointLight->isLit())
                {
                    pointLight->turnOff();
                }
                else
                {
                    pointLight->turnOn();
                }
            }
            index++;
        }

        ImGui::End();
    }

  private:
    std::vector<std::shared_ptr<Spark::Drawable>> m_cubes;
    Spark::Camera m_camera;
    bool m_focused;
    bool m_addingBox;
    bool m_removingBox;
    bool m_setDirLight;
    bool m_setSpotLight;
    bool m_addingLightBox;
    bool m_removingLightBox;
    float m_nextCords[3];
    float m_nextLightsCords[3];
    float m_dirLightDirection[3];
    float m_dirLightColor[3];
    float m_spotLightColor[3];
    float m_pointLightColor[3];
    int m_removeBoxIndex;
    int m_previousRemoveBoxIndex;
    int m_removeLightIndex;
    int m_previousRemoveLightIndex;
    std::vector<std::shared_ptr<Spark::PointLight>> m_pointLights;
};

class Sandbox : public Spark::Application
{
  public:
    Sandbox()
        : layer()
        , m_layer2D(false)
    {
        layer = std::make_unique<Sandbox3DLayer>();
        layer2d = std::make_unique<SandboxLayer>();
        PushLayer(layer.get());
    }

    virtual void generateOverlay()
    {
        Spark::Application::generateOverlay();

        ImGui::SetNextWindowSize(ImVec2(350, 170), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 170), ImGuiCond_Always);
        ImGui::Begin("Sandbox", NULL,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        if (ImGui::Checkbox("2d layer", &m_layer2D))
        {
            if (m_layer2D)
            {
                PushLayerBefore(layer2d.get(), layer.get());
            }
            else
            {
                PopLayer(layer2d.get());
            }
        }

        ImGui::End();

        layer->generateOverlay();
    }

    bool handleKeyPressed(Spark::KeyPressedEvent &e)
    {
        return false;
    }

    virtual void handleEvent(Spark::Event &e)
    {
        Application::handleEvent(e);
        Spark::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox::handleKeyPressed));
    }

  private:
    std::unique_ptr<Sandbox3DLayer> layer;
    std::unique_ptr<SandboxLayer> layer2d;
    bool m_layer2D;
};

Spark::Application *Spark::CreateApplication()
{
    return new Sandbox();
}
