#include "sandbox_3d.h"

#include <limits>

static std::vector<glm::vec3> buildCircle(float radius, int sectors)
{
    std::vector<glm::vec3> points;
    float sectorStep = 2 * glm::pi<float>() / sectors;
    for (int i = 0; i < sectors; ++i)
    {
        float sectorAngle = i * sectorStep; // starting from 0 to 2pi
        float x = radius * glm::cos(sectorAngle);
        float y = radius * glm::sin(sectorAngle);
        points.push_back({x, y, 0});
    }
    return points;
}

Sandbox3D::Sandbox3D(Spark::Application &app, Spark::Layer3D &layer, Spark::Render::LockedCamera &playCamera,
                     Spark::Render::FreeCamera &editorCamera)
    : m_app(app)
    , m_layer(layer)
    , m_editorLayer(editorCamera)
    , m_objects()
    , m_pointLights()
    , m_playCamera(playCamera)
    , m_editorCamera(editorCamera)
    , m_inEditor(true)
    , m_objectToSet(nullptr)
    , m_pointLightToSet(nullptr)
    , m_panning(false)
    , m_rotating(false)
{
    const Spark::Texture &texture = Spark::ResourceManager::loadTexture("cubeTexture", "textures/container2.png");
    const Spark::Texture &specularTexture =
        Spark::ResourceManager::loadTexture("cubeTextureSpecular", "textures/container2_specular.png");
    // m_objects.push_back(Spark::createBox({0, 0, 0}, 1, 1, 1, texture, specularTexture));
    // addObjectA(*m_objects.back());
    const Spark::Model &model = Spark::ResourceManager::loadModel("modelTest", "model/backpack/scene.gltf", true);
    m_objects.push_back(Spark::createModelObject(model, {0, 0, 2}, {0.005, 0.005, 0.005}));
    m_layer.addObject(*m_objects.back());

    m_objects.push_back(Spark::createBox(glm::vec3(0, 0, 0), 1, 1, 1, texture, specularTexture));
    m_layer.addObject(*m_objects.back());
    m_objects.push_back(Spark::createBox(glm::vec3(0, 1, 0), 1, 1, 1, texture, specularTexture));
    m_layer.addObject(*m_objects.back());
    // m_objects.push_back(
    //     Spark::createPipe({{0, 0, 0}, {1, 0, 0}, {2, 1, 0}, {3, 1, 0}, {3, 0, 0}, {2, -1, 0}, {1, -1, 0}, {0, 0, 0}},
    //                       0.1f, texture, specularTexture));
    // m_objects.push_back(Spark::createPipe(buildCircle(0.5f, 64), 0.02f, true, {1, 0, 0}));
    // addObject(*m_objects.back());
    // m_objects.back()->rotate(glm::radians(90.0f), {0, 1, 0});
    // m_objects.push_back(Spark::createPipe(buildCircle(0.5f, 64), 0.02f, true, {0, 1, 0}));
    // addObject(*m_objects.back());
    // m_objects.back()->rotate(glm::radians(90.0f), {1, 0, 0});
    // m_objects.push_back(Spark::createPipe(buildCircle(0.5f, 64), 0.02f, true, {0, 0, 1}));
    // addObject(*m_objects.back());
    // m_objects.back()->setScale({2, 2, 2});
    // m_objects.back()->getDrawable()->setCalculateLight(false);
    // m_objects.push_back(Spark::createPipe({{2, 0, 0}, {2, 1, 0}}, 0.1f, false, texture, specularTexture));
    // addObject(*m_objects.back());

    // m_objects.push_back(Spark::createCylinder(glm::vec3(0, 0, 0), 0.5, 0.5, 2, {0, 0, 1}));
    // addObject(*m_objects.back());
    // Spark::Object3D *arrowBody = m_objects.back().get();
    // m_objects.push_back(Spark::createCylinder(glm::vec3(0, 0, 1.5f), 1, 0, 1, {0, 0, 1}));
    // addObject(*m_objects.back());
    // arrowBody->addChild(m_objects.back());

    // m_objects.push_back(Spark::createCylinder(glm::vec3(0, 0, 0), 0.5, 0.5, 2, {1, 0, 0}));
    // addObject(*m_objects.back());
    // arrowBody = m_objects.back().get();
    // m_objects.push_back(Spark::createCylinder(glm::vec3(0, 0, 1.5f), 1, 0, 1, {1, 0, 0}));
    // addObject(*m_objects.back());
    // m_objects.back()->setParent(arrowBody);
    // arrowBody->rotate(glm::radians(90.0f), {1, 0, 0});

    // glm::vec3 spherePos = {0, 0, 0.0f};
    // std::unique_ptr<Spark::Object3D> sphere = Spark::createSphere(spherePos, 0.15f, glm::vec3(0.3f, 0.3f, 0.3f));
    // m_pointLights.push_back(Spark::Render::createPointLight(spherePos, {0, 1, 0}, std::move(sphere), false));
    // m_objects.push_back(m_pointLights.back());
    // addPointLight(*(m_pointLights.back()));
}

void Sandbox3D::OnUpdate(Spark::Time &diffTime)
{
    if (!m_inEditor)
    {
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::A))
        {
            m_playCamera.moveDirection(Spark::Render::CameraDirection::LEFT, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::D))
        {
            m_playCamera.moveDirection(Spark::Render::CameraDirection::RIGHT, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::W))
        {
            m_playCamera.moveDirection(Spark::Render::CameraDirection::FORWARD, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::S))
        {
            m_playCamera.moveDirection(Spark::Render::CameraDirection::BACKWARD, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::LeftShift))
        {
            m_playCamera.moveDirection(Spark::Render::CameraDirection::UP, diffTime);
        }
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::LeftControl))
        {
            m_playCamera.moveDirection(Spark::Render::CameraDirection::DOWN, diffTime);
        }
    }
}

void Sandbox3D::OnEvent(Spark::Event &e)
{
    Spark::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Spark::MouseMovedEvent>(SPARK_BIND_EVENT_FN(Sandbox3D::handleMouseMoved));
    dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox3D::handleKeyPressed));
    dispatcher.Dispatch<Spark::KeyReleasedEvent>(SPARK_BIND_EVENT_FN(Sandbox3D::handleKeyReleased));
    dispatcher.Dispatch<Spark::MouseButtonPressedEvent>(SPARK_BIND_EVENT_FN(Sandbox3D::handleMousePressed));
    dispatcher.Dispatch<Spark::MouseButtonReleasedEvent>(SPARK_BIND_EVENT_FN(Sandbox3D::handleMouseReleased));
    dispatcher.Dispatch<Spark::MouseScrolledEvent>(SPARK_BIND_EVENT_FN(Sandbox3D::handleMouseScroll));
}

bool Sandbox3D::isInEditor() const
{
    return m_inEditor;
}

void Sandbox3D::addBox()
{
    const Spark::Texture *texture = Spark::ResourceManager::getTexture("cubeTexture");
    const Spark::Texture *specularTexture = Spark::ResourceManager::getTexture("cubeTextureSpecular");
    m_objects.push_back(Spark::createBox(glm::vec3(0, 0, 0), 1, 1, 1, *texture, *specularTexture));
    m_layer.addObject(*m_objects.back());
    deselectObject();
    m_objectToSet = m_objects.back().get();
    m_objectToSet->getDrawable()->highlight();
    addEditor(*m_objectToSet);
}

void Sandbox3D::addObject()
{
    const Spark::Model *model = Spark::ResourceManager::getModel("modelTest");
    m_objects.push_back(Spark::createModelObject(*model, {0, 0, 0}, {0.005, 0.005, 0.005}));
    m_layer.addObject(*m_objects.back());
    deselectObject();
    m_objectToSet = m_objects.back().get();
    m_objectToSet->getDrawable()->highlight();
    addEditor(*m_objectToSet);
}

void Sandbox3D::addPointLight(PointLightType type, const glm::vec3 &position, const glm::vec3 &color)
{
    if (type == PointLightType::Sphere)
    {
        std::unique_ptr<Spark::Object3D> sphere =
            std::make_unique<Spark::Sphere>(position, 0.15f, glm::vec3(0.3f, 0.3f, 0.3f));
        m_pointLights.push_back(Spark::Render::createPointLight(position, color, std::move(sphere)));
    }
    else
    {
        std::unique_ptr<Spark::Object3D> cube = Spark::createBox(position, 0.3f, 0.3f, 0.3f, {0.3f, 0.3f, 0.3f});
        m_pointLights.push_back(Spark::Render::createPointLight(position, color, std::move(cube)));
    }
    m_layer.addPointLight(*(m_pointLights.back()));
    m_objects.push_back(m_pointLights.back());
    selectPointLight(m_pointLights.back().get());
}

void Sandbox3D::removePointLight(Spark::Render::PointLight *pointLight)
{
    m_layer.removePointLight(*pointLight);
    m_objects.erase(std::find_if(m_objects.begin(), m_objects.end(),
                                 [&](std::shared_ptr<Spark::Object3D> &p) { return p.get() == pointLight; }));
    m_pointLights.erase(
        std::find_if(m_pointLights.begin(), m_pointLights.end(),
                     [&](std::shared_ptr<Spark::Render::PointLight> &p) { return p.get() == pointLight; }));
    m_pointLightToSet = nullptr;
    removeEditor();
}

Spark::Object3D *Sandbox3D::getObjectToSet()
{
    return m_objectToSet;
}

Spark::Render::PointLight *Sandbox3D::getPointLightToSet()
{
    return m_pointLightToSet;
}

const std::vector<std::shared_ptr<Spark::Render::PointLight>> &Sandbox3D::getPointLights()
{
    return m_pointLights;
}

void Sandbox3D::run()
{
    Spark::Input::HideMouse();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    ImGui::SetWindowFocus(NULL);
    m_inEditor = false;
    m_layer.setCamera(m_playCamera);
    deselectObject();
}

void Sandbox3D::pause()
{
    Spark::Input::UnHideMouse();
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    m_inEditor = true;
    m_layer.setCamera(m_editorCamera);
}

bool Sandbox3D::handleMouseMoved(Spark::MouseMovedEvent &e)
{
    if (m_inEditor)
    {
        if (m_panning)
        {
            m_editorCamera.moveDirection(Spark::Render::CameraDirection::DOWN, e.GetDiffY() * 0.01f);
            m_editorCamera.moveDirection(Spark::Render::CameraDirection::LEFT, e.GetDiffX() * 0.01f);
            return true;
        }
        else if (m_rotating)
        {
            glm::vec3 front = glm::rotate(glm::mat4(1), glm::radians(-e.GetDiffX() * 1.0f), m_editorCamera.getUp()) *
                              glm::vec4(m_editorCamera.getFront(), 1);
            glm::vec3 position = glm::rotate(glm::mat4(1), glm::radians(-e.GetDiffX() * 1.0f), m_editorCamera.getUp()) *
                                 glm::vec4(m_editorCamera.getPosition(), 1);
            front = glm::rotate(glm::mat4(1), glm::radians(e.GetDiffY() * 1.0f), m_editorCamera.getRight()) *
                    glm::vec4(front, 1);
            position = glm::rotate(glm::mat4(1), glm::radians(e.GetDiffY() * 1.0f), m_editorCamera.getRight()) *
                       glm::vec4(position, 1);
            if (glm::acos(glm::dot(front, m_editorCamera.getUp())) > 0.1 &&
                glm::acos(glm::dot(front, m_editorCamera.getUp())) < glm::pi<float>() - 0.1)
            {
                m_editorCamera.setFrontUp(front, m_editorCamera.getUp());
                m_editorCamera.setPosition(position);
            }
        }
        return false;
    }

    m_playCamera.moveAngle(e.GetDiffX(), e.GetDiffY());
    return true;
}

bool Sandbox3D::handleKeyPressed(Spark::KeyPressedEvent &e)
{
    switch (e.GetKeyCode())
    {
    case Spark::KeyCode::Escape:
        return escape();
    case Spark::KeyCode::Space:
        return toggleRun();
    case Spark::KeyCode::Delete:
        return removeSelectedObject();
    case Spark::KeyCode::LeftShift:
        if (Spark::Input::IsMouseButtonPressed(Spark::MouseCode::ButtonMiddle))
        {
            m_rotating = false;
            m_panning = true;
        }
    default:
        return false;
    }
}

bool Sandbox3D::handleKeyReleased(Spark::KeyReleasedEvent &e)
{
    switch (e.GetKeyCode())
    {
    case Spark::KeyCode::LeftShift:
        if (Spark::Input::IsMouseButtonPressed(Spark::MouseCode::ButtonMiddle))
        {
            m_panning = false;
            m_rotating = true;
        }
    default:
        return false;
    }
}

bool Sandbox3D::handleMousePressed(Spark::MouseButtonPressedEvent &e)
{
    switch (e.GetMouseButton())
    {
    case Spark::MouseCode::ButtonLeft:
        return pickObject();
    case Spark::MouseCode::ButtonMiddle:
        if (Spark::Input::IsKeyPressed(Spark::KeyCode::LeftShift))
        {
            m_panning = true;
        }
        else
        {
            m_rotating = true;
        }
        return true;
    default:
        return false;
    }
}

bool Sandbox3D::handleMouseReleased(Spark::MouseButtonReleasedEvent &e)
{
    switch (e.GetMouseButton())
    {
    case Spark::MouseCode::ButtonMiddle:
        m_panning = false;
        m_rotating = false;
        return true;
    default:
        return false;
    }
}

bool Sandbox3D::handleMouseScroll(Spark::MouseScrolledEvent &e)
{
    if (!m_inEditor)
    {
        return false;
    }

    m_editorCamera.zoom(e.GetYOffset());
    return true;
}

void Sandbox3D::addEditor(Spark::Object3D &object)
{
    m_editorLayer.setObjectToEdit(object);
    if (!m_editorLayer.isAttached())
    {
        m_app.PushLayer(&m_editorLayer);
    }
}

void Sandbox3D::removeEditor()
{
    if (m_editorLayer.isAttached())
    {
        m_app.PopLayer(&m_editorLayer);
    }
}

void Sandbox3D::selectObject(Spark::Object3D *object)
{
    deselectObject();
    m_objectToSet = object;
    m_objectToSet->getDrawable()->highlight();
    addEditor(*m_objectToSet);
}

void Sandbox3D::selectPointLight(Spark::Render::PointLight *pointLight)
{
    deselectObject();
    m_pointLightToSet = pointLight;
    m_pointLightToSet->getDrawable()->highlight();
    addEditor(*m_pointLightToSet);
}

bool Sandbox3D::deselectObject()
{
    if (m_pointLightToSet)
    {
        m_pointLightToSet->getDrawable()->unhighlight();
        m_pointLightToSet = nullptr;
        removeEditor();
    }
    else if (m_objectToSet)
    {
        m_objectToSet->getDrawable()->unhighlight();
        m_objectToSet = nullptr;
        removeEditor();
    }
    else
    {
        return false;
    }
    return true;
}

bool Sandbox3D::escape()
{
    if (!m_inEditor)
    {
        pause();
        return true;
    }
    else
    {
        if (!deselectObject())
        {
            return false;
        }

        return true;
    }
}

bool Sandbox3D::toggleRun()
{
    if (m_inEditor)
    {
        run();
    }
    else
    {
        pause();
    }
    return true;
}

bool Sandbox3D::removeSelectedObject()
{
    if (m_inEditor)
    {
        if (m_pointLightToSet)
        {
            m_layer.removePointLight(*m_pointLightToSet);
            m_pointLights.erase(std::find_if(
                m_pointLights.begin(), m_pointLights.end(),
                [&](std::shared_ptr<Spark::Render::PointLight> &p) { return p.get() == m_pointLightToSet; }));
            m_objects.erase(std::find_if(m_objects.begin(), m_objects.end(), [&](std::shared_ptr<Spark::Object3D> &p) {
                return p.get() == m_pointLightToSet;
            }));
            m_pointLightToSet = nullptr;
            removeEditor();
            return true;
        }
        else if (m_objectToSet)
        {
            m_layer.removeObject(*m_objectToSet);
            m_objects.erase(std::find_if(m_objects.begin(), m_objects.end(), [&](std::shared_ptr<Spark::Object3D> &p) {
                return p.get() == m_objectToSet;
            }));
            m_objectToSet = nullptr;
            removeEditor();
            return true;
        }
    }
    return false;
}

bool Sandbox3D::pickObject()
{
    if (!m_inEditor)
    {
        return false;
    }
    Spark::Object3D *closestObject = nullptr;
    float closest = std::numeric_limits<float>::max();
    for (auto &object : m_objects)
    {
        float distance = Spark::Physics::getRayDistanceFromObject(Spark::Physics::getMouseRay(m_editorCamera),
                                                                  object->getPhysicsObject());
        if (distance > 0 && distance < closest)
        {
            closest = distance;
            closestObject = object.get();
        }
    }
    if (closestObject)
    {
        while (closestObject->getParent())
        {
            closestObject = closestObject->getParent();
        }
        if (m_objectToSet)
        {
            deselectObject();
        }
        if (m_pointLightToSet)
        {
            deselectObject();
        }
        closestObject->getDrawable()->highlight();
        auto isLight = std::find_if(m_pointLights.begin(), m_pointLights.end(),
                                    [&](std::shared_ptr<Spark::Render::PointLight> &p) {
                                        return p.get() == closestObject;
                                    }) != m_pointLights.end();
        if (isLight)
        {
            selectPointLight(static_cast<Spark::Render::PointLight *>(closestObject));
        }
        else
        {
            selectObject(closestObject);
        }

        addEditor(*closestObject);
    }
    return true;
}
