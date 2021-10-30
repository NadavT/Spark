#include "editor_3d_rotate.h"

static const glm::vec3 X_COLOR = {0.5f, 0, 0};
static const glm::vec3 X_HIGHLIGHT_COLOR = {1, 0, 0};
static const glm::vec3 Y_COLOR = {0, 0.5f, 0};
static const glm::vec3 Y_HIGHLIGHT_COLOR = {0, 1, 0};
static const glm::vec3 Z_COLOR = {0, 0, 0.5f};
static const glm::vec3 Z_HIGHLIGHT_COLOR = {0, 0, 1};

Editor3DRotate::Editor3DRotate(Spark::Layer3D &layer)
    : m_layer(layer)
    , m_xRing(nullptr)
    , m_yRing(nullptr)
    , m_zRing(nullptr)
    , m_selectedObject(nullptr)
    , m_selectedAxis(Axis::X)
    , m_originalRotation(0)
{
    m_xRing = createRing(X_COLOR);
    m_xRing->rotate(glm::radians(90.0f), {0, 1, 0});
    m_xRing->setAsRelativeTransform();

    m_yRing = createRing(Y_COLOR);
    m_yRing->rotate(glm::radians(90.0f), {1, 0, 0});
    m_yRing->setAsRelativeTransform();

    m_zRing = createRing(Z_COLOR);
    m_zRing->setAsRelativeTransform();
}

float Editor3DRotate::findSelectedRotate(Spark::Render::Camera &camera)
{
    float closest = std::numeric_limits<float>::max();
    int currentAxis = static_cast<int>(Axis::X);
    for (auto &object : {m_xRing, m_yRing, m_zRing})
    {
        float distance =
            Spark::Physics::getRayDistanceFromObject(Spark::Physics::getMouseRay(camera), object->getPhysicsObject());
        if (distance > 0 && (closest == -1 || distance < closest))
        {
            closest = distance;
            m_selectedObject = object.get();
            m_selectedAxis = static_cast<Axis>(currentAxis);
        }
        currentAxis++;
    }
    return closest;
}

void Editor3DRotate::initializeRotateTransform(Spark::Render::Camera &camera)
{
    Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(camera);
    glm::vec3 direction;
    switch (m_selectedAxis)
    {
    case Axis::X:
        if (glm::dot(mouseRay.direction, {1, 0, 0}) == 0)
        {
            m_selectedObject = nullptr;
            return;
        }
        m_xRing->getDrawable()->setColor(X_HIGHLIGHT_COLOR, true);
        direction = Spark::getIntersectionNormalRay(m_xRing->getPhysicsObject().getPosition(), {1, 0, 0}, mouseRay) -
                    m_xRing->getPhysicsObject().getPosition();
        m_originalRotation =
            glm::acos(glm::dot({0, 0, 1}, direction) / (glm::length(glm::vec3(0, 0, 1)) * glm::length(direction)));
        if (direction.y < 0)
        {
            m_originalRotation = 2 * glm::pi<float>() - m_originalRotation;
        }
        break;
    case Axis::Y:
        if (glm::dot(mouseRay.direction, {0, 1, 0}) == 0)
        {
            m_selectedObject = nullptr;
            return;
        }
        m_yRing->getDrawable()->setColor(Y_HIGHLIGHT_COLOR, true);
        direction = Spark::getIntersectionNormalRay(m_xRing->getPhysicsObject().getPosition(), {0, 1, 0}, mouseRay) -
                    m_yRing->getPhysicsObject().getPosition();
        m_originalRotation =
            glm::acos(glm::dot({0, 0, 1}, direction) / (glm::length(glm::vec3(0, 0, 1)) * glm::length(direction)));
        if (direction.x > 0)
        {
            m_originalRotation = 2 * glm::pi<float>() - m_originalRotation;
        }
        break;
    case Axis::Z:
        if (glm::dot(mouseRay.direction, {0, 0, 1}) == 0)
        {
            m_selectedObject = nullptr;
            return;
        }
        m_zRing->getDrawable()->setColor(Z_HIGHLIGHT_COLOR, true);
        direction = Spark::getIntersectionNormalRay(m_xRing->getPhysicsObject().getPosition(), {0, 0, 1}, mouseRay) -
                    m_zRing->getPhysicsObject().getPosition();
        m_originalRotation =
            glm::acos(glm::dot({1, 0, 0}, direction) / (glm::length(glm::vec3(1, 0, 0)) * glm::length(direction)));
        if (direction.y > 0)
        {
            m_originalRotation = 2 * glm::pi<float>() - m_originalRotation;
        }
        break;
        // case Axis::CUSTOM:
        //     m_viewRing->getDrawable()->setColor(VIEW_HIGHLIGHT_COLOR, true);
        //     direction = Spark::getIntersectionNormalRay(m_viewRing->getPhysicsObject().getPosition(),
        //     m_camera.getFront(),
        //                                                 mouseRay) -
        //                 m_viewRing->getPhysicsObject().getPosition();
        //     m_originalRotatation =
        //         glm::acos(glm::dot({1, 0, 0}, direction) / (glm::length(glm::vec3(1, 0, 0)) *
        //         glm::length(direction)));
        //     if (direction.y > 0)
        //     {
        //         m_originalRotatation = 2 * glm::pi<float>() - m_originalRotatation;
        //     }
        //     break;
    }
}

void Editor3DRotate::handleRotateTransformUpdate(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit)
{
    Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(camera);
    glm::vec3 direction;
    float angle;
    switch (m_selectedAxis)
    {
    case Axis::X:
        if (glm::dot(mouseRay.direction, {1, 0, 0}) == 0)
        {
            return;
        }
        direction = Spark::getIntersectionNormalRay(m_xRing->getPhysicsObject().getPosition(), {1, 0, 0}, mouseRay) -
                    m_xRing->getPhysicsObject().getPosition();
        angle = glm::acos(glm::dot({0, 0, 1}, direction) / (glm::length(glm::vec3(0, 0, 1)) * glm::length(direction)));
        if (direction.y < 0)
        {
            angle = 2 * glm::pi<float>() - angle;
        }
        objectToEdit->rotate(m_originalRotation - angle, {1, 0, 0});
        break;
    case Axis::Y:
        if (glm::dot(mouseRay.direction, {0, 1, 0}) == 0)
        {
            return;
        }
        direction = Spark::getIntersectionNormalRay(m_yRing->getPhysicsObject().getPosition(), {0, 1, 0}, mouseRay) -
                    m_yRing->getPhysicsObject().getPosition();
        angle = glm::acos(glm::dot({0, 0, 1}, direction) / (glm::length(glm::vec3(0, 0, 1)) * glm::length(direction)));
        if (direction.x > 0)
        {
            angle = 2 * glm::pi<float>() - angle;
        }
        objectToEdit->rotate(m_originalRotation - angle, {0, 1, 0});
        break;
    case Axis::Z:
        if (glm::dot(mouseRay.direction, {0, 0, 1}) == 0)
        {
            return;
        }
        direction = Spark::getIntersectionNormalRay(m_zRing->getPhysicsObject().getPosition(), {0, 0, 1}, mouseRay) -
                    m_zRing->getPhysicsObject().getPosition();
        angle = glm::acos(glm::dot({1, 0, 0}, direction) / (glm::length(glm::vec3(1, 0, 0)) * glm::length(direction)));
        if (direction.y > 0)
        {
            angle = 2 * glm::pi<float>() - angle;
        }
        objectToEdit->rotate(m_originalRotation - angle, {0, 0, 1});
        break;
    }
    m_originalRotation = angle;
}

void Editor3DRotate::updateObjects(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit)
{
    auto distanceVector = camera.getPosition() - m_xRing->getPhysicsObject().getPosition();
    auto planeNormal = glm::normalize(camera.getFront());
    float relative = glm::abs(glm::dot(distanceVector, planeNormal));
    m_xRing->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_yRing->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_zRing->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_xRing->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_yRing->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_zRing->setPosition(objectToEdit->getPhysicsObject().getPosition());
}

void Editor3DRotate::addTransforms()
{
    m_layer.addObjectAndChilds(*m_xRing);
    m_layer.addObjectAndChilds(*m_yRing);
    m_layer.addObjectAndChilds(*m_zRing);
}

void Editor3DRotate::removeTransforms()
{
    m_layer.removeObjectAndChilds(*m_xRing);
    m_layer.removeObjectAndChilds(*m_yRing);
    m_layer.removeObjectAndChilds(*m_zRing);
}

void Editor3DRotate::release()
{
    m_selectedObject = nullptr;
    m_xRing->getDrawable()->setColor(X_COLOR, true);
    m_yRing->getDrawable()->setColor(Y_COLOR, true);
    m_zRing->getDrawable()->setColor(Z_COLOR, true);
}

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

std::shared_ptr<Spark::Object3D> Editor3DRotate::createRing(glm::vec3 color)
{
    std::shared_ptr<Spark::Object3D> ring = Spark::createPipe(buildCircle(2.5f, 64), 0.05f, true, color);
    auto ringBound = std::make_unique<Spark::Physics::Pipe>(buildCircle(2.5f, 64), 0.1f, true);
    ring->setPhysicsObject(std::move(ringBound));
    ring->getDrawable()->setCalculateLight(false);
    return ring;
}
