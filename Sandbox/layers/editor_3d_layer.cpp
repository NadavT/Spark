#include "editor_3d_layer.h"

Editor3DLayer::Editor3DLayer(Spark::Render::Camera &camera)
    : Spark::Layer3D("Editor 3d layer", camera)
    , m_camera(camera)
    , m_objectToEdit(nullptr)
    , m_selectedArrow(nullptr)
    , m_selectedAxis(Axis::X)
    , m_originalAxisPosition(0)
{

    m_xArrow = createArrow({1, 0, 0});
    m_xArrow->rotate(glm::radians(90.0f), {0, 1, 0});
    m_xArrow->setAsRelativeTransform();
    float relative = std::max(glm::distance(m_camera.getPosition(), m_xArrow->getPhysicsObject().getPosition()), 2.5f);
    m_xArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});

    m_yArrow = createArrow({0, 1, 0});
    m_yArrow->rotate(glm::radians(270.0f), {1, 0, 0});
    m_yArrow->setAsRelativeTransform();
    m_yArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});

    m_zArrow = createArrow({0, 0, 1});
    m_zArrow->setAsRelativeTransform();
    m_zArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
}

void Editor3DLayer::OnAttach()
{
    Layer3D::OnAttach();
}

void Editor3DLayer::OnDetach()
{
    Layer3D::OnDetach();
}

void Editor3DLayer::OnUpdate(Spark::Time &diffTime)
{
    Layer3D::OnUpdate(diffTime);
    auto distanceVector = m_camera.getPosition() - m_xArrow->getPhysicsObject().getPosition();
    auto planeNormal = glm::normalize(m_camera.getFront());
    float relative = glm::abs(glm::dot(distanceVector, planeNormal));
    m_xArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_yArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_zArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_xArrow->setPosition(m_objectToEdit->getPhysicsObject().getPosition());
    m_yArrow->setPosition(m_objectToEdit->getPhysicsObject().getPosition());
    m_zArrow->setPosition(m_objectToEdit->getPhysicsObject().getPosition());
}

void Editor3DLayer::OnEvent(Spark::Event &e)
{
    Layer3D::OnEvent(e);
    Spark::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Spark::MouseMovedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleMouseMoved));
    dispatcher.Dispatch<Spark::KeyPressedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleKeyPressed));
    dispatcher.Dispatch<Spark::MouseButtonPressedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleMousePressed));
    dispatcher.Dispatch<Spark::MouseButtonReleasedEvent>(SPARK_BIND_EVENT_FN(Editor3DLayer::handleMouseReleased));
}

void Editor3DLayer::setObjectToEdit(Spark::Object3D &object)
{
    m_objectToEdit = &object;
}

std::shared_ptr<Spark::Object3D> Editor3DLayer::createArrow(glm::vec3 color)
{
    std::shared_ptr<Spark::Object3D> arrowBody = Spark::createCylinder(glm::vec3(0, 0, 0), 0.2f, 0.2f, 2, color);
    addObject(*arrowBody);
    std::shared_ptr<Spark::Object3D> arrowHead = Spark::createCylinder(glm::vec3(0, 0, 1.25f), 0.5f, 0, 0.5f, color);
    addObject(*arrowHead);
    arrowBody->addChild(arrowHead);
    arrowHead->getDrawable()->setCalculateLight(false);
    arrowBody->move({0, 0, 1});
    arrowBody->setAsRelativeTransform();
    arrowBody->getDrawable()->setCalculateLight(false);
    return arrowBody;
}

bool Editor3DLayer::handleMouseMoved(Spark::MouseMovedEvent &e)
{
    if (m_selectedArrow)
    {
        Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(m_camera);
        float newPosition = 0;
        switch (m_selectedAxis)
        {
        case Axis::X:
            newPosition = Spark::Physics::getClosestPointToRayFromRay(
                              {{1, 0, 0}, m_xArrow->getPhysicsObject().getPosition()}, mouseRay)
                              .x;
            m_objectToEdit->move({newPosition - m_originalAxisPosition, 0, 0});
            m_originalAxisPosition = newPosition;
            break;
        case Axis::Y:
            newPosition = Spark::Physics::getClosestPointToRayFromRay(
                              {{0, 1, 0}, m_yArrow->getPhysicsObject().getPosition()}, mouseRay)
                              .y;
            m_objectToEdit->move({0, newPosition - m_originalAxisPosition, 0});
            m_originalAxisPosition = newPosition;
            break;
        case Axis::Z:
            newPosition = Spark::Physics::getClosestPointToRayFromRay(
                              {{0, 0, 1}, m_zArrow->getPhysicsObject().getPosition()}, mouseRay)
                              .z;
            m_objectToEdit->move({0, 0, newPosition - m_originalAxisPosition});
            m_originalAxisPosition = newPosition;
            break;
        }
    }

    return false;
}

bool Editor3DLayer::handleKeyPressed(Spark::KeyPressedEvent &e)
{
    return false;
}

bool Editor3DLayer::handleMousePressed(Spark::MouseButtonPressedEvent &e)
{
    float closest = std::numeric_limits<float>::max();
    int currentAxis = static_cast<int>(Axis::X);
    switch (e.GetMouseButton())
    {
    case Spark::MouseCode::ButtonLeft:
        m_selectedArrow = nullptr;
        for (auto &object : {m_xArrow, m_yArrow, m_zArrow})
        {
            float distance = Spark::Physics::getRayDistanceFromObject(Spark::Physics::getMouseRay(m_camera),
                                                                      object->getPhysicsObject());
            if (distance > 0 && distance < closest)
            {
                closest = distance;
                m_selectedArrow = object.get();
                m_selectedAxis = static_cast<Axis>(currentAxis);
            }
            currentAxis++;
        }

        if (m_selectedArrow)
        {
            Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(m_camera);
            switch (m_selectedAxis)
            {
            case Axis::X:
                m_originalAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                             {{1, 0, 0}, m_xArrow->getPhysicsObject().getPosition()}, mouseRay)
                                             .x;
                break;
            case Axis::Y:
                m_originalAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                             {{0, 1, 0}, m_yArrow->getPhysicsObject().getPosition()}, mouseRay)
                                             .y;
                break;
            case Axis::Z:
                m_originalAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                             {{0, 0, 1}, m_zArrow->getPhysicsObject().getPosition()}, mouseRay)
                                             .z;
                break;
            }
            SPARK_INFO("Selected ray axis {0}, position: {1}", m_selectedAxis, m_originalAxisPosition);
            return true;
        }
    default:
        return false;
    }
}

bool Editor3DLayer::handleMouseReleased(Spark::MouseButtonReleasedEvent &e)
{
    m_selectedArrow = nullptr;

    return false;
}
