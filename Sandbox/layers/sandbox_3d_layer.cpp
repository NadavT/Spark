#include "sandbox_3d_layer.h"

Sandbox3DLayer::Sandbox3DLayer()
    : m_camera({10.0f, 0.0f, 0.0f})
    , Spark::Layer3D("Sandbox 3d layer", m_camera)
    , m_drawables()
    , m_pointLights()
    , m_focused(false)
    , m_lightType(0)
    , m_removeBoxIndex(0)
    , m_previousRemoveBoxIndex(0)
    , m_removePointLightIndex(0)
    , m_previousRemoveLightIndex(0)
    , m_dirLightOn(true)
    , m_spotLightOn(true)
    , m_wireframe(0)
    , m_nextCords{0, 0, 0}
    , m_dirLightDirection{-0.2f, -1.0f, -0.3f}
    , m_dirLightColor{1, 1, 1}
    , m_spotLightColor{1, 1, 1}
    , m_nextPointLightCords{0, 0, 0}
    , m_pointLightColor{1, 1, 1}
    , m_wireframeColor{0, 0, 0}
    , m_beforeWireframeColor{0, 0, 0}
{
    const Spark::Texture &texture = Spark::ResourceManager::loadTexture("cubeTexutre", "textures/container2.png");
    const Spark::Texture &specularTexture =
        Spark::ResourceManager::loadTexture("cubeTexutreSpecular", "textures/container2_specular.png");
    m_drawables.push_back(std::move(Spark::createCube({0, 0, 0}, texture, specularTexture)));
    addDrawable(m_drawables[0]);
    setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
    m_pointLights.push_back(Spark::createPointLight(
        {0, 0, -2.0f}, {0, 1, 0}, Spark::createSphere({0, 0, -2.0f}, {0.3f, 0.3f, 0.3f}, 36, 18, {0.3f, 0.3f, 0.3f})));
    addPointLight(*(m_pointLights.back()));
    setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
}

void Sandbox3DLayer::OnAttach()
{
    SPARK_INFO("Attached {0}", getName());
    Layer3D::OnAttach();
}

void Sandbox3DLayer::OnDetach()
{
    Spark::Input::UnHideMouse();
    Layer3D::OnDetach();
    SPARK_INFO("Detached {0}", getName());
}

void Sandbox3DLayer::OnUpdate(Spark::Time &diffTime)
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

void Sandbox3DLayer::OnEvent(Spark::Event &e)
{
    Layer3D::OnEvent(e);
    Spark::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Spark::MouseMovedEvent>(SPARK_BIND_EVENT_FN(Sandbox3DLayer::handleMouseMoved));
    dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox3DLayer::handleKeyPressed));
    dispatcher.Dispatch<Spark::MouseButtonPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox3DLayer::handleMousePressed));
}

void Sandbox3DLayer::generateOverlay()
{
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 20, 20), ImGuiCond_Once, ImVec2(1, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(250, 100), ImVec2(250, ImGui::GetIO().DisplaySize.y));
    ImGui::Begin("3d editor", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

    if (ImGui::TreeNode("Objects editor:"))
    {
        generateBoxAdder();
        generateBoxRemover();
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("Lights editor:"))
    {
        generateDirLightSetter();
        generateSpotLightSetter();
        generatePointLightAdder();
        generatePointLightRemover();
        generatePointLightsFlicker();
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("Wireframe editor:"))
    {
        generateWireframeSetter();
        ImGui::TreePop();
    }

    ImGui::End();
}

bool Sandbox3DLayer::handleMouseMoved(Spark::MouseMovedEvent &e)
{
    if (!m_focused)
    {
        return false;
    }

    m_camera.moveAngle(e.GetDiffX(), e.GetDiffY());
    return true;
}

bool Sandbox3DLayer::handleKeyPressed(Spark::KeyPressedEvent &e)
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

bool Sandbox3DLayer::handleMousePressed(Spark::MouseButtonPressedEvent &e)
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

void Sandbox3DLayer::generateBoxAdder()
{
    if (ImGui::Button("add box"))
    {
        SPARK_INFO("Adding box");
        ImGui::OpenPopup("Box adder");
    }

    if (ImGui::BeginPopup("Box adder"))
    {
        ImGui::InputFloat3("", m_nextCords);
        ImGui::SameLine();
        if (ImGui::Button("add"))
        {
            const Spark::Texture *texture = Spark::ResourceManager::getTexture("cubeTexutre");
            const Spark::Texture *specularTexture = Spark::ResourceManager::getTexture("cubeTexutreSpecular");
            m_drawables.push_back(std::move(
                Spark::createCube({m_nextCords[0], m_nextCords[1], m_nextCords[2]}, *texture, *specularTexture)));
            addDrawable(m_drawables.back());
            for (int i = 0; i < 3; i++)
            {
                m_nextCords[i] = 0;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            for (int i = 0; i < 3; i++)
            {
                m_nextCords[i] = 0;
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Sandbox3DLayer::generateBoxRemover()
{
    if (ImGui::Button("remove box"))
    {
        SPARK_INFO("Removing box");
        if (m_drawables.size() > 0)
        {
            m_drawables[0].get()->highlight();
        }
        ImGui::OpenPopup("Box remover");
    }

    if (ImGui::BeginPopup("Box remover"))
    {
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("", &m_removeBoxIndex))
        {
            if (m_removeBoxIndex >= 0 && m_removeBoxIndex < m_drawables.size())
            {
                m_drawables[m_previousRemoveBoxIndex].get()->unhighlight();
                m_drawables[m_removeBoxIndex].get()->highlight();
                m_previousRemoveBoxIndex = m_removeBoxIndex;
            }
            else if (m_removeBoxIndex < 0)
            {
                m_removeBoxIndex = 0;
                m_drawables[m_previousRemoveBoxIndex].get()->unhighlight();
                m_drawables[m_removeBoxIndex].get()->highlight();
                m_previousRemoveBoxIndex = m_removeBoxIndex;
            }
            else
            {
                m_removeBoxIndex = static_cast<int>(m_drawables.size()) - 1;
                m_drawables[m_previousRemoveBoxIndex].get()->unhighlight();
                m_drawables[m_removeBoxIndex].get()->highlight();
                m_previousRemoveBoxIndex = m_removeBoxIndex;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("remove"))
        {
            removeDrawable(m_drawables[m_removeBoxIndex].get());
            m_drawables.erase(m_drawables.begin() + m_removeBoxIndex);
            m_removeBoxIndex = 0;
            m_previousRemoveBoxIndex = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            if (m_drawables.size() > m_removeBoxIndex)
            {
                m_drawables[m_removeBoxIndex].get()->unhighlight();
            }
            m_removeBoxIndex = 0;
            m_previousRemoveBoxIndex = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else
    {
        if (m_drawables.size() > m_removeBoxIndex)
        {
            m_drawables[m_removeBoxIndex].get()->unhighlight();
        }
        m_removeBoxIndex = 0;
        m_previousRemoveBoxIndex = 0;
    }
}

void Sandbox3DLayer::generateDirLightSetter()
{
    if (ImGui::Checkbox("dir light", &m_dirLightOn))
    {
        if (m_dirLightOn)
        {
            SPARK_INFO("Turning on dir light");
            setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                        {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
        }
        else
        {
            SPARK_INFO("Turning off dir light");
            setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]}, {0, 0, 0});
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("set dir light"))
    {
        SPARK_INFO("Setting dir light");
        ImGui::OpenPopup("Dir light setter");
    }

    if (ImGui::BeginPopup("Dir light setter"))
    {
        ImGui::InputFloat3("direction", m_dirLightDirection);
        ImGui::ColorEdit3("color", m_dirLightColor);
        if (ImGui::Button("set"))
        {
            setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                        {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Sandbox3DLayer::generateSpotLightSetter()
{
    if (ImGui::Checkbox("spot light", &m_spotLightOn))
    {
        if (m_spotLightOn)
        {
            SPARK_INFO("Turning on spot light");
            setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
        }
        else
        {
            SPARK_INFO("Turning off spot light");
            setSpotLight({0, 0, 0});
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("set spot light"))
    {
        SPARK_INFO("Setting spot light");
        ImGui::OpenPopup("Spot light setter");
    }

    if (ImGui::BeginPopup("Spot light setter"))
    {
        ImGui::ColorEdit3("color", m_spotLightColor);
        if (ImGui::Button("set"))
        {
            setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Sandbox3DLayer::generatePointLightAdder()
{
    if (ImGui::Button("add point light"))
    {
        SPARK_INFO("Adding point light");
        ImGui::OpenPopup("Point light adder");
    }

    if (ImGui::BeginPopup("Point light adder"))
    {
        ImGui::InputFloat3("location", m_nextPointLightCords);
        ImGui::ColorEdit3("color", m_pointLightColor);
        ImGui::Combo("type", &m_lightType, "Sphere\0Cube\0");
        if (ImGui::Button("add"))
        {
            std::shared_ptr<Spark::Drawable> drawable = NULL;
            if (m_lightType == 0)
            {
                drawable =
                    Spark::createSphere({m_nextPointLightCords[0], m_nextPointLightCords[1], m_nextPointLightCords[2]},
                                        {0.3f, 0.3f, 0.3f}, 36, 18, {0.3f, 0.3f, 0.3f});
            }
            else
            {
                drawable =
                    Spark::createCube({m_nextPointLightCords[0], m_nextPointLightCords[1], m_nextPointLightCords[2]},
                                      {0.3f, 0.3f, 0.3f}, {0.3f, 0.3f, 0.3f});
            }
            m_pointLights.push_back(
                Spark::createPointLight({m_nextPointLightCords[0], m_nextPointLightCords[1], m_nextPointLightCords[2]},
                                        {m_pointLightColor[0], m_pointLightColor[1], m_pointLightColor[2]}, drawable));
            addPointLight(*(m_pointLights.back()));
            for (int i = 0; i < 3; i++)
            {
                m_nextPointLightCords[i] = 0;
                m_pointLightColor[i] = 1;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            for (int i = 0; i < 3; i++)
            {
                m_nextPointLightCords[i] = 0;
                m_pointLightColor[i] = 1;
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Sandbox3DLayer::generatePointLightRemover()
{
    if (ImGui::Button("remove point light"))
    {
        SPARK_INFO("Removing point light");
        m_removePointLightIndex = 0;
        m_previousRemoveLightIndex = 0;
        if (m_pointLights.size() > 0)
        {
            m_pointLights[0].get()->drawable.get()->highlight();
        }
        ImGui::OpenPopup("Point light remover");
    }

    if (ImGui::BeginPopup("Point light remover"))
    {
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("", &m_removePointLightIndex))
        {
            if (m_removePointLightIndex >= 0 && m_removePointLightIndex < m_pointLights.size())
            {
                m_pointLights[m_previousRemoveLightIndex].get()->drawable.get()->unhighlight();
                m_pointLights[m_removePointLightIndex].get()->drawable.get()->highlight();
                m_previousRemoveLightIndex = m_removePointLightIndex;
            }
            else if (m_removePointLightIndex < 0)
            {
                m_removePointLightIndex = 0;
                m_pointLights[m_previousRemoveLightIndex].get()->drawable.get()->unhighlight();
                m_pointLights[m_removePointLightIndex].get()->drawable.get()->highlight();
                m_previousRemoveLightIndex = m_removePointLightIndex;
            }
            else
            {
                m_removePointLightIndex = static_cast<int>(m_pointLights.size()) - 1;
                m_pointLights[m_previousRemoveLightIndex].get()->drawable.get()->unhighlight();
                m_pointLights[m_removePointLightIndex].get()->drawable.get()->highlight();
                m_previousRemoveLightIndex = m_removePointLightIndex;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("remove"))
        {
            removePointLight(*(m_pointLights[m_removePointLightIndex].get()));
            m_pointLights.erase(m_pointLights.begin() + m_removePointLightIndex);
            m_removePointLightIndex = 0;
            m_previousRemoveLightIndex = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            if (m_pointLights.size() > m_removePointLightIndex)
            {
                m_pointLights[m_removePointLightIndex].get()->drawable.get()->unhighlight();
            }
            m_removePointLightIndex = 0;
            m_previousRemoveLightIndex = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else
    {
        if (m_pointLights.size() > m_removePointLightIndex)
        {
            m_pointLights[m_removePointLightIndex].get()->drawable.get()->unhighlight();
        }
        m_removePointLightIndex = 0;
        m_previousRemoveLightIndex = 0;
    }
}

void Sandbox3DLayer::generateWireframeSetter()
{
    ImGui::SetNextItemWidth(100);
    if (ImGui::Combo("type", &m_wireframe, "None\0Only\0Both\0"))
    {
        setWireframe(static_cast<Spark::WireframeState>(m_wireframe),
                     {m_wireframeColor[0], m_wireframeColor[1], m_wireframeColor[2]});
    }
    ImGui::SameLine();
    if (ImGui::Button("set color"))
    {
        m_beforeWireframeColor[0] = m_wireframeColor[0];
        m_beforeWireframeColor[1] = m_wireframeColor[1];
        m_beforeWireframeColor[2] = m_wireframeColor[2];
        ImGui::OpenPopup("Wireframe color setter");
    }
    if (ImGui::BeginPopup("Wireframe color setter"))
    {
        if (ImGui::ColorEdit3("color", m_wireframeColor))
        {
            setWireframe(static_cast<Spark::WireframeState>(m_wireframe),
                         {m_wireframeColor[0], m_wireframeColor[1], m_wireframeColor[2]});
        }
        if (ImGui::Button("set"))
        {
            m_beforeWireframeColor[0] = m_wireframeColor[0];
            m_beforeWireframeColor[1] = m_wireframeColor[1];
            m_beforeWireframeColor[2] = m_wireframeColor[2];
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            m_wireframeColor[0] = m_beforeWireframeColor[0];
            m_wireframeColor[1] = m_beforeWireframeColor[1];
            m_wireframeColor[2] = m_beforeWireframeColor[2];
            setWireframe(static_cast<Spark::WireframeState>(m_wireframe),
                         {m_wireframeColor[0], m_wireframeColor[1], m_wireframeColor[2]});
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else
    {
        m_wireframeColor[0] = m_beforeWireframeColor[0];
        m_wireframeColor[1] = m_beforeWireframeColor[1];
        m_wireframeColor[2] = m_beforeWireframeColor[2];
        setWireframe(static_cast<Spark::WireframeState>(m_wireframe),
                     {m_wireframeColor[0], m_wireframeColor[1], m_wireframeColor[2]});
    }
}

void Sandbox3DLayer::generatePointLightsFlicker()
{
    int index = 0;
    for (auto &pointLight : m_pointLights)
    {
        bool lit = pointLight->isLit();
        if (ImGui::Checkbox(("Point light " + std::to_string(index)).c_str(), &lit))
        {
            if (lit)
            {
                pointLight->turnOn();
            }
            else
            {
                pointLight->turnOff();
            }
        }
        index++;
    }
}
