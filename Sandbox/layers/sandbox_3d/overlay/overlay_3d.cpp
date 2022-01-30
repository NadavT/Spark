#include "overlay_3d.h"

Overlay3D::Overlay3D(Spark::Layer3D &layer, Sandbox3D &sandbox)
    : m_layer(layer)
    , m_sandbox(sandbox)
    , m_lightType(0)
    , m_dirLightOn(true)
    , m_spotLightOn(true)
    , m_wireframe(static_cast<int>(Spark::Render::WireframeState::None))
    , m_dirLightDirection{-0.2f, -1.0f, -0.3f}
    , m_dirLightColor{1, 1, 1}
    , m_spotLightColor{0, 0, 0}
    , m_wireframeColor{0, 0, 0}
    , m_beforeWireframeColor{0, 0, 0}
{
    m_layer.setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                        {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
    m_layer.setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
}

void Overlay3D::generateOverlay()
{
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 20, 20), ImGuiCond_Once, ImVec2(1, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(250, 100), ImVec2(250, ImGui::GetIO().DisplaySize.y));
    ImGui::Begin("3d editor", NULL,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoNavInputs);

    if (ImGui::TreeNode("Objects editor:"))
    {
        generateBoxAdder();
        ImGui::TreePop();
    }
    generateObjectSetter();

    ImGui::Separator();

    if (ImGui::TreeNode("Lights editor:"))
    {
        generateDirLightSetter();
        generateSpotLightSetter();
        generatePointLightAdder();
        generatePointLightsSelector();
        ImGui::TreePop();
    }
    generatePointLightSetter();

    ImGui::Separator();

    if (ImGui::TreeNode("Wireframe editor:"))
    {
        generateWireframeSetter();
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("Highlight:"))
    {
        generateHighlightSetter();
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::Button("Run"))
    {
        m_sandbox.run();
    }

    ImGui::End();
}

void Overlay3D::generateBoxAdder()
{
    if (ImGui::Button("add box"))
    {
        SPARK_INFO("Adding box");
        m_sandbox.addBox();
    }
}

void Overlay3D::generateObjectSetter()
{
    Spark::Object3D *objectToSet = m_sandbox.getObjectToSet();
    if (objectToSet)
    {
        glm::vec3 position = objectToSet->getPhysicsObject().getPosition();
        std::array<float, 3> nextObjectCords = {position.x, position.y, position.z};
        static float rotationDegree = 0;
        static std::array<bool, 3> rotationAxis = {0, 0, 1};
        ImGui::CaptureKeyboardFromApp(false);
        ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
        ImGui::Begin("Object setter", NULL,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavInputs);
        if (ImGui::InputFloat3("location", nextObjectCords.data()))
        {
            objectToSet->setPosition({nextObjectCords[0], nextObjectCords[1], nextObjectCords[2]});
        }
        ImGui::Separator();
        ImGui::InputFloat("degree", &rotationDegree);
        ImGui::Text("Axis:");
        ImGui::SameLine();
        ImGui::Checkbox("x", &rotationAxis[0]);
        ImGui::SameLine();
        ImGui::Checkbox("y", &rotationAxis[1]);
        ImGui::SameLine();
        ImGui::Checkbox("z", &rotationAxis[2]);
        if (ImGui::Button("rotate"))
        {
            objectToSet->rotate(glm::radians(rotationDegree),
                                glm::vec3(rotationAxis[0], rotationAxis[1], rotationAxis[2]));
        }
        ImGui::Separator();
        if (ImGui::Button("set"))
        {
            m_sandbox.deselectObject();
        }

        ImGui::End();
    }
}

void Overlay3D::generateDirLightSetter()
{
    if (ImGui::Checkbox("dir light", &m_dirLightOn))
    {
        if (m_dirLightOn)
        {
            SPARK_INFO("Turning on dir light");
            m_layer.setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                                {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
        }
        else
        {
            SPARK_INFO("Turning off dir light");
            m_layer.setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]}, {0, 0, 0});
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("set dir light"))
    {
        SPARK_INFO("Setting dir light");
        m_beforeDirLightDirection = m_dirLightDirection;
        m_beforeDirLightColor = m_dirLightColor;
        ImGui::OpenPopup("Dir light setter");
    }

    if (ImGui::BeginPopup("Dir light setter"))
    {
        ImGui::InputFloat3("direction", m_dirLightDirection.data());
        if (ImGui::ColorEdit3("color", m_dirLightColor.data()))
        {
            m_layer.setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                                {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
        }
        if (ImGui::Button("set"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            m_dirLightDirection = m_beforeDirLightDirection;
            m_dirLightColor = m_beforeDirLightColor;
            m_layer.setDirLight({m_dirLightDirection[0], m_dirLightDirection[1], m_dirLightDirection[2]},
                                {m_dirLightColor[0], m_dirLightColor[1], m_dirLightColor[2]});
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Overlay3D::generateSpotLightSetter()
{
    if (ImGui::Checkbox("spot light", &m_spotLightOn))
    {
        if (m_spotLightOn)
        {
            SPARK_INFO("Turning on spot light");
            m_layer.setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
        }
        else
        {
            SPARK_INFO("Turning off spot light");
            m_layer.setSpotLight({0, 0, 0});
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("set spot light"))
    {
        SPARK_INFO("Setting spot light");
        m_beforeSpotLightColor = m_spotLightColor;
        ImGui::OpenPopup("Spot light setter");
    }

    if (ImGui::BeginPopup("Spot light setter"))
    {
        if (ImGui::ColorEdit3("color", m_spotLightColor.data()))
        {
            m_layer.setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
        }
        if (ImGui::Button("set"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            m_spotLightColor = m_beforeSpotLightColor;
            m_layer.setSpotLight({m_spotLightColor[0], m_spotLightColor[1], m_spotLightColor[2]});
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Overlay3D::generatePointLightAdder()
{
    if (ImGui::Button("add point light"))
    {
        SPARK_INFO("Adding point light");
        m_sandbox.addPointLight(Sandbox3D::PointLightType::Sphere);
    }
}

void Overlay3D::generatePointLightSetter()
{
    Spark::Render::PointLight *pointLightToSet = m_sandbox.getPointLightToSet();
    if (pointLightToSet)
    {
        std::array<float, 3> nextPointCords = {pointLightToSet->getPosition().x, pointLightToSet->getPosition().y,
                                               pointLightToSet->getPosition().z};
        std::array<float, 3> nextPointColor = {pointLightToSet->getColor().x, pointLightToSet->getColor().y,
                                               pointLightToSet->getColor().z};
        bool isLit = pointLightToSet->isLit();
        ImGui::CaptureKeyboardFromApp(false);
        ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
        ImGui::Begin("Point light setter", NULL,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavInputs);
        if (ImGui::InputFloat3("location", nextPointCords.data()))
        {
            pointLightToSet->setPosition({nextPointCords[0], nextPointCords[1], nextPointCords[2]});
        }
        if (ImGui::ColorEdit3("color##PointLight", nextPointColor.data()))
        {
            pointLightToSet->setColor({nextPointColor[0], nextPointColor[1], nextPointColor[2]});
        }
        if (ImGui::Combo("type", &m_lightType, "Sphere\0Cube\0"))
        {
            m_sandbox.removePointLight(pointLightToSet);
            m_sandbox.addPointLight((m_lightType) ? Sandbox3D::PointLightType::Cube : Sandbox3D::PointLightType::Sphere,
                                    {nextPointCords[0], nextPointCords[1], nextPointCords[2]},
                                    {nextPointColor[0], nextPointColor[1], nextPointColor[2]});
        }
        if (ImGui::Checkbox("lit", &isLit))
        {
            if (isLit)
            {
                pointLightToSet->turnOn();
            }
            else
            {
                pointLightToSet->turnOff();
            }
        }

        if (ImGui::Button("set"))
        {
            m_sandbox.deselectObject();
        }

        ImGui::End();
    }
}

void Overlay3D::generatePointLightsSelector()
{
    int index = 0;
    for (auto &pointLight : m_sandbox.getPointLights())
    {
        if (ImGui::Button(("set Point light " + std::to_string(index)).c_str()))
        {
            m_sandbox.selectPointLight(pointLight.get());
        }
        index++;
    }
}

void Overlay3D::generateWireframeSetter()
{
    ImGui::SetNextItemWidth(100);
    if (ImGui::Combo("##wireframe type", &m_wireframe, "None\0Only\0Both\0"))
    {
        m_layer.setWireframe(static_cast<Spark::Render::WireframeState>(m_wireframe),
                             {m_wireframeColor[0], m_wireframeColor[1], m_wireframeColor[2]});
    }
    ImGui::SameLine();
    if (ImGui::Button("set color"))
    {
        m_beforeWireframeColor = m_wireframeColor;
        ImGui::OpenPopup("Wireframe color setter");
    }
    if (ImGui::BeginPopup("Wireframe color setter"))
    {
        if (ImGui::ColorEdit3("color", m_wireframeColor.data()))
        {
            m_layer.setWireframe(static_cast<Spark::Render::WireframeState>(m_wireframe),
                                 {m_wireframeColor[0], m_wireframeColor[1], m_wireframeColor[2]});
        }
        if (ImGui::Button("set"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("cancel"))
        {
            m_wireframeColor = m_beforeWireframeColor;
            m_layer.setWireframe(static_cast<Spark::Render::WireframeState>(m_wireframe),
                                 {m_wireframeColor[0], m_wireframeColor[1], m_wireframeColor[2]});
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Overlay3D::generateHighlightSetter()
{
    bool isXrayHighlight = m_layer.getXrayHighlight();
    if (ImGui::Checkbox("x-ray highlight", &isXrayHighlight))
    {
        m_layer.setXrayHighlight(isXrayHighlight);
    }
}
