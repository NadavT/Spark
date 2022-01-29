#include "editor_3d_scale.h"

#include "editor_utils.h"

static const glm::vec3 X_COLOR = {0.5f, 0, 0};
static const glm::vec3 X_HIGHLIGHT_COLOR = {1, 0, 0};
static const glm::vec3 Y_COLOR = {0, 0.5f, 0};
static const glm::vec3 Y_HIGHLIGHT_COLOR = {0, 1, 0};
static const glm::vec3 Z_COLOR = {0, 0, 0.5f};
static const glm::vec3 Z_HIGHLIGHT_COLOR = {0, 0, 1};
static const glm::vec3 VIEW_COLOR = {0.5f, 0.5f, 0.5f};
static const glm::vec3 VIEW_HIGHLIGHT_COLOR = {1, 1, 1};

Editor3DScale::Editor3DScale(Spark::Layer3D &layer)
    : m_layer(layer)
    , m_xArrow(nullptr)
    , m_yArrow(nullptr)
    , m_zArrow(nullptr)
    , m_viewRing(nullptr)
    , m_selectedObject(nullptr)
    , m_selectedAxis(Axis::X)
    , m_originalScaleAxisPosition(0)
{
    m_xArrow = createArrow(X_COLOR, ArrowHead::Cube);
    m_xArrow->rotate(glm::radians(90.0f), {0, 1, 0});
    m_xArrow->setAsRelativeTransform();

    m_yArrow = createArrow(Y_COLOR, ArrowHead::Cube);
    m_yArrow->rotate(glm::radians(270.0f), {1, 0, 0});
    m_yArrow->setAsRelativeTransform();

    m_zArrow = createArrow(Z_COLOR, ArrowHead::Cube);
    m_zArrow->setAsRelativeTransform();

    m_viewRing = createRing(VIEW_COLOR);
    m_viewRing->scale({1.25f, 1.25f, 1.25f});
    m_viewRing->setAsRelativeTransform();
}

float Editor3DScale::findSelectedScale(Spark::Render::Camera &camera)
{
    float closest = -1;
    int currentAxis = static_cast<int>(Axis::X);
    for (auto &object : {m_xArrow, m_yArrow, m_zArrow, m_viewRing})
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

void Editor3DScale::initializeScaleTransform(Spark::Render::Camera &camera)
{
    Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(camera);
    float distance;
    switch (m_selectedAxis)
    {
    case Axis::X:
        m_xArrow->getDrawable()->setColor(X_HIGHLIGHT_COLOR, true);
        m_originalScaleAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                          {{1, 0, 0}, m_xArrow->getPhysicsObject().getPosition()}, mouseRay)
                                          .x;
        break;
    case Axis::Y:
        m_yArrow->getDrawable()->setColor(Y_HIGHLIGHT_COLOR, true);
        m_originalScaleAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                          {{0, 1, 0}, m_yArrow->getPhysicsObject().getPosition()}, mouseRay)
                                          .y;
        break;
    case Axis::Z:
        m_zArrow->getDrawable()->setColor(Z_HIGHLIGHT_COLOR, true);
        m_originalScaleAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                          {{0, 0, 1}, m_zArrow->getPhysicsObject().getPosition()}, mouseRay)
                                          .z;
        break;
    case Axis::View:
        if (glm::dot(mouseRay.direction, camera.getFront()) == 0)
        {
            m_selectedObject = nullptr;
            return;
        }
        m_viewRing->getDrawable()->setColor(VIEW_HIGHLIGHT_COLOR, true);

        m_originalScaleAxisPosition = glm::length(
            Spark::getIntersectionNormalRay(m_viewRing->getPhysicsObject().getPosition(), camera.getFront(), mouseRay) -
            m_viewRing->getPhysicsObject().getPosition());
        break;
    }
}

void Editor3DScale::handleScaleTransformUpdate(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit)
{
    Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(camera);
    float newPosition = 0;
    switch (m_selectedAxis)
    {
    case Axis::X:
        newPosition = Spark::Physics::getClosestPointToRayFromRay(
                          {{1, 0, 0}, m_xArrow->getPhysicsObject().getPosition()}, mouseRay)
                          .x;
        objectToEdit->scale({1 + newPosition - m_originalScaleAxisPosition, 1, 1});
        m_originalScaleAxisPosition = newPosition;
        break;
    case Axis::Y:
        newPosition = Spark::Physics::getClosestPointToRayFromRay(
                          {{0, 1, 0}, m_yArrow->getPhysicsObject().getPosition()}, mouseRay)
                          .y;
        objectToEdit->scale({1, 1 + newPosition - m_originalScaleAxisPosition, 1});
        m_originalScaleAxisPosition = newPosition;
        break;
    case Axis::Z:
        newPosition = Spark::Physics::getClosestPointToRayFromRay(
                          {{0, 0, 1}, m_zArrow->getPhysicsObject().getPosition()}, mouseRay)
                          .z;
        objectToEdit->scale({1, 1, 1 + newPosition - m_originalScaleAxisPosition});
        m_originalScaleAxisPosition = newPosition;
        break;
    case Axis::View:
        if (glm::dot(mouseRay.direction, camera.getFront()) == 0)
        {
            m_selectedObject = nullptr;
            return;
        }

        newPosition = glm::length(
            Spark::getIntersectionNormalRay(m_viewRing->getPhysicsObject().getPosition(), camera.getFront(), mouseRay) -
            m_viewRing->getPhysicsObject().getPosition());
        objectToEdit->scale({1 + newPosition - m_originalScaleAxisPosition,
                             1 + newPosition - m_originalScaleAxisPosition,
                             1 + newPosition - m_originalScaleAxisPosition});
        m_originalScaleAxisPosition = newPosition;
        break;
    }
}

void Editor3DScale::updateObjects(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit)
{
    auto distanceVector = camera.getPosition() - m_xArrow->getPhysicsObject().getPosition();
    auto planeNormal = glm::normalize(camera.getFront());
    float relative = glm::abs(glm::dot(distanceVector, planeNormal));
    m_xArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_yArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_zArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_viewRing->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_xArrow->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_yArrow->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_zArrow->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_viewRing->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_xArrow->setRotation(objectToEdit->getPhysicsObject().getRotation());
    m_yArrow->setRotation(objectToEdit->getPhysicsObject().getRotation());
    m_zArrow->setRotation(objectToEdit->getPhysicsObject().getRotation());

    float angle = glm::acos(glm::dot(camera.getFront(), {0, 0, 1})) / (glm::length(glm::vec3(camera.getFront())));
    glm::vec3 axis = glm::cross(glm::vec3(0, 0, 1), camera.getFront());
    m_viewRing->setRotation(angle, axis);
}

void Editor3DScale::addTransforms()
{
    m_layer.addObjectAndChilds(*m_xArrow);
    m_layer.addObjectAndChilds(*m_yArrow);
    m_layer.addObjectAndChilds(*m_zArrow);
    m_layer.addObjectAndChilds(*m_viewRing);
}

void Editor3DScale::removeTransforms()
{
    m_layer.removeObjectAndChilds(*m_xArrow);
    m_layer.removeObjectAndChilds(*m_yArrow);
    m_layer.removeObjectAndChilds(*m_zArrow);
    m_layer.removeObjectAndChilds(*m_viewRing);
}

void Editor3DScale::release()
{
    m_selectedObject = nullptr;
    m_xArrow->getDrawable()->setColor(X_COLOR, true);
    m_yArrow->getDrawable()->setColor(Y_COLOR, true);
    m_zArrow->getDrawable()->setColor(Z_COLOR, true);
    m_viewRing->getDrawable()->setColor(VIEW_COLOR, true);
}
