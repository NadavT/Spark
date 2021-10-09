#include "editor_3d_layer.h"

#include <memory>

static const glm::vec3 X_COLOR = {0.5f, 0, 0};
static const glm::vec3 X_HIGHLIGHT_COLOR = {1, 0, 0};
static const glm::vec3 Y_COLOR = {0, 0.5f, 0};
static const glm::vec3 Y_HIGHLIGHT_COLOR = {0, 1, 0};
static const glm::vec3 Z_COLOR = {0, 0, 0.5f};
static const glm::vec3 Z_HIGHLIGHT_COLOR = {0, 0, 1};

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

Editor3DLayer::Editor3DLayer(Spark::Render::Camera &camera)
    : Spark::Layer3D("Editor 3d layer", camera)
    , m_camera(camera)
    , m_objectToEdit(nullptr)
    , m_selectedObject(nullptr)
    , m_selectedTransform(Transform::Move)
    , m_selectedAxis(Axis::X)
    , m_originalAxisPosition(0)
{

    m_xArrow = createArrow(X_COLOR);
    m_xArrow->rotate(glm::radians(90.0f), {0, 1, 0});
    m_xArrow->setAsRelativeTransform();

    m_yArrow = createArrow(Y_COLOR);
    m_yArrow->rotate(glm::radians(270.0f), {1, 0, 0});
    m_yArrow->setAsRelativeTransform();

    m_zArrow = createArrow(Z_COLOR);
    m_zArrow->setAsRelativeTransform();

    m_xRing = createRing(X_COLOR);
    m_xRing->rotate(glm::radians(90.0f), {0, 1, 0});
    m_xRing->setAsRelativeTransform();

    m_yRing = createRing(Y_COLOR);
    m_yRing->rotate(glm::radians(90.0f), {1, 0, 0});
    m_yRing->setAsRelativeTransform();

    m_zRing = createRing(Z_COLOR);
    m_zRing->setAsRelativeTransform();
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
    m_xRing->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_yRing->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_zRing->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_xArrow->setPosition(m_objectToEdit->getPhysicsObject().getPosition());
    m_yArrow->setPosition(m_objectToEdit->getPhysicsObject().getPosition());
    m_zArrow->setPosition(m_objectToEdit->getPhysicsObject().getPosition());
    m_xRing->setPosition(m_objectToEdit->getPhysicsObject().getPosition());
    m_yRing->setPosition(m_objectToEdit->getPhysicsObject().getPosition());
    m_zRing->setPosition(m_objectToEdit->getPhysicsObject().getPosition());
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
    auto boxBound = std::make_unique<Spark::Physics::Box>(glm::vec3(0, 0, 0), 1.0f, 1.0f, 2.5f);
    arrowBody->setPhysicsObject(std::move(boxBound));
    arrowBody->move({0, 0, 1});
    arrowBody->setAsRelativeTransform();
    arrowBody->getDrawable()->setCalculateLight(false);
    return arrowBody;
}

std::shared_ptr<Spark::Object3D> Editor3DLayer::createRing(glm::vec3 color)
{
    std::shared_ptr<Spark::Object3D> ring = Spark::createPipe(buildCircle(2.5f, 64), 0.05f, true, color);
    addObject(*ring);
    auto ringBound = std::make_unique<Spark::Physics::Pipe>(buildCircle(2.5f, 64), 0.1f, true);
    ring->setPhysicsObject(std::move(ringBound));
    ring->getDrawable()->setCalculateLight(false);
    return ring;
}

bool Editor3DLayer::handleMouseMoved(Spark::MouseMovedEvent &e)
{
    if (m_selectedObject)
    {
        Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(m_camera);
        float newPosition = 0;
        switch (m_selectedTransform)
        {
        case Transform::Move:
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
            break;
        case Transform::Rotate:
            glm::vec3 direction;
            float angle;
            switch (m_selectedAxis)
            {
            case Axis::X:
                direction = Spark::getIntersectionNormalRay({0, 0, 0}, {1, 0, 0}, mouseRay);
                angle = glm::acos(glm::dot({0, 0, 1}, direction) /
                                  (glm::length(glm::vec3(0, 0, 1)) * glm::length(direction)));
                if (direction.y < 0)
                {
                    angle = 2 * glm::pi<float>() - angle;
                }
                m_objectToEdit->rotate(m_originalAxisPosition - angle, {1, 0, 0});
                m_originalAxisPosition = angle;
                break;
            case Axis::Y:
                direction = Spark::getIntersectionNormalRay({0, 0, 0}, {0, 1, 0}, mouseRay);
                angle = glm::acos(glm::dot({0, 0, 1}, direction) /
                                  (glm::length(glm::vec3(0, 0, 1)) * glm::length(direction)));
                if (direction.x > 0)
                {
                    angle = 2 * glm::pi<float>() - angle;
                }
                m_objectToEdit->rotate(m_originalAxisPosition - angle, {0, 1, 0});
                m_originalAxisPosition = angle;
                break;
            case Axis::Z:
                direction = Spark::getIntersectionNormalRay({0, 0, 0}, {0, 0, 1}, mouseRay);
                angle = glm::acos(glm::dot({1, 0, 0}, direction) /
                                  (glm::length(glm::vec3(1, 0, 0)) * glm::length(direction)));
                if (direction.y > 0)
                {
                    angle = 2 * glm::pi<float>() - angle;
                }
                m_objectToEdit->rotate(m_originalAxisPosition - angle, {0, 0, 1});
                m_originalAxisPosition = angle;
                break;
            }
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
        m_selectedObject = nullptr;
        for (auto &object : {m_xArrow, m_yArrow, m_zArrow})
        {
            float distance = Spark::Physics::getRayDistanceFromObject(Spark::Physics::getMouseRay(m_camera),
                                                                      object->getPhysicsObject());
            if (distance > 0 && distance < closest)
            {
                closest = distance;
                m_selectedObject = object.get();
                m_selectedTransform = Transform::Move;
                m_selectedAxis = static_cast<Axis>(currentAxis);
            }
            currentAxis++;
        }
        currentAxis = static_cast<int>(Axis::X);
        for (auto &object : {m_xRing, m_yRing, m_zRing})
        {
            float distance = Spark::Physics::getRayDistanceFromObject(Spark::Physics::getMouseRay(m_camera),
                                                                      object->getPhysicsObject());
            if (distance > 0 && distance < closest)
            {
                closest = distance;
                m_selectedObject = object.get();
                m_selectedTransform = Transform::Rotate;
                m_selectedAxis = static_cast<Axis>(currentAxis);
            }
            currentAxis++;
        }

        if (m_selectedObject)
        {
            Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(m_camera);
            switch (m_selectedTransform)
            {
            case Transform::Move:
                switch (m_selectedAxis)
                {
                case Axis::X:
                    m_xArrow->getDrawable()->setColor(X_HIGHLIGHT_COLOR, true);
                    m_originalAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                                 {{1, 0, 0}, m_xArrow->getPhysicsObject().getPosition()}, mouseRay)
                                                 .x;
                    break;
                case Axis::Y:
                    m_yArrow->getDrawable()->setColor(Y_HIGHLIGHT_COLOR, true);
                    m_originalAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                                 {{0, 1, 0}, m_yArrow->getPhysicsObject().getPosition()}, mouseRay)
                                                 .y;
                    break;
                case Axis::Z:
                    m_zArrow->getDrawable()->setColor(Z_HIGHLIGHT_COLOR, true);
                    m_originalAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                                 {{0, 0, 1}, m_zArrow->getPhysicsObject().getPosition()}, mouseRay)
                                                 .z;
                    break;
                }
                SPARK_INFO("Selected ray axis {0}, position: {1}", m_selectedAxis, m_originalAxisPosition);
                return true;
            case Transform::Rotate:
                glm::vec3 direction;
                switch (m_selectedAxis)
                {
                case Axis::X:
                    m_xRing->getDrawable()->setColor(X_HIGHLIGHT_COLOR, true);
                    direction = Spark::getIntersectionNormalRay({0, 0, 0}, {1, 0, 0}, mouseRay);
                    m_originalAxisPosition = glm::acos(glm::dot({0, 0, 1}, direction) /
                                                       (glm::length(glm::vec3(0, 0, 1)) * glm::length(direction)));
                    if (direction.y < 0)
                    {
                        m_originalAxisPosition = 2 * glm::pi<float>() - m_originalAxisPosition;
                    }
                    break;
                case Axis::Y:
                    m_yRing->getDrawable()->setColor(Y_HIGHLIGHT_COLOR, true);
                    direction = Spark::getIntersectionNormalRay({0, 0, 0}, {0, 1, 0}, mouseRay);
                    m_originalAxisPosition = glm::acos(glm::dot({0, 0, 1}, direction) /
                                                       (glm::length(glm::vec3(0, 0, 1)) * glm::length(direction)));
                    if (direction.x > 0)
                    {
                        m_originalAxisPosition = 2 * glm::pi<float>() - m_originalAxisPosition;
                    }
                    break;
                case Axis::Z:
                    m_zRing->getDrawable()->setColor(Z_HIGHLIGHT_COLOR, true);
                    direction = Spark::getIntersectionNormalRay({0, 0, 0}, {0, 0, 1}, mouseRay);
                    m_originalAxisPosition = glm::acos(glm::dot({1, 0, 0}, direction) /
                                                       (glm::length(glm::vec3(1, 0, 0)) * glm::length(direction)));
                    if (direction.y > 0)
                    {
                        m_originalAxisPosition = 2 * glm::pi<float>() - m_originalAxisPosition;
                    }
                    break;
                }
                SPARK_INFO("Selected ray axis {0}, position: {1}", m_selectedAxis, m_originalAxisPosition);
                return true;
            }
        }
    default:
        return false;
    }
}

bool Editor3DLayer::handleMouseReleased(Spark::MouseButtonReleasedEvent &e)
{
    m_selectedObject = nullptr;
    m_xArrow->getDrawable()->setColor(X_COLOR, true);
    m_yArrow->getDrawable()->setColor(Y_COLOR, true);
    m_zArrow->getDrawable()->setColor(Z_COLOR, true);
    m_xRing->getDrawable()->setColor(X_COLOR, true);
    m_yRing->getDrawable()->setColor(Y_COLOR, true);
    m_zRing->getDrawable()->setColor(Z_COLOR, true);

    return false;
}
