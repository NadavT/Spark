#include "editor_3d_move.h"

#include "editor_3d_utils.h"

static const glm::vec3 X_COLOR = {0.5f, 0, 0};
static const glm::vec3 X_HIGHLIGHT_COLOR = {1, 0, 0};
static const glm::vec3 Y_COLOR = {0, 0.5f, 0};
static const glm::vec3 Y_HIGHLIGHT_COLOR = {0, 1, 0};
static const glm::vec3 Z_COLOR = {0, 0, 0.5f};
static const glm::vec3 Z_HIGHLIGHT_COLOR = {0, 0, 1};

Editor3DMove::Editor3DMove(Spark::Layer3D &layer)
    : m_layer(layer)
    , m_xArrowObject(nullptr)
    , m_yArrowObject(nullptr)
    , m_zArrowObject(nullptr)
    , m_minimalXArrowObject(nullptr)
    , m_minimalYArrowObject(nullptr)
    , m_minimalZArrowObject(nullptr)
    , m_xArrow(nullptr)
    , m_yArrow(nullptr)
    , m_zArrow(nullptr)
    , m_selectedObject(nullptr)
    , m_selectedAxis(Axis::X)
    , m_originalMoveAxisPosition(0)
    , m_minimal(false)
    , m_addObjects(false)
{
    m_xArrowObject = createArrow(X_COLOR, ArrowHead::Cone);
    m_xArrowObject->rotate(glm::radians(90.0f), {0, 1, 0});
    m_xArrowObject->setAsRelativeTransform();
    m_minimalXArrowObject = createArrowHead(X_COLOR, ArrowHead::Cone, 3);
    m_minimalXArrowObject->rotate(glm::radians(90.0f), {0, 1, 0});
    m_minimalXArrowObject->setAsRelativeTransform();
    m_xArrow = m_xArrowObject.get();

    m_yArrowObject = createArrow(Y_COLOR, ArrowHead::Cone);
    m_yArrowObject->rotate(glm::radians(270.0f), {1, 0, 0});
    m_yArrowObject->setAsRelativeTransform();
    m_minimalYArrowObject = createArrowHead(Y_COLOR, ArrowHead::Cone, 3);
    m_minimalYArrowObject->rotate(glm::radians(270.0f), {1, 0, 0});
    m_minimalYArrowObject->setAsRelativeTransform();
    m_yArrow = m_yArrowObject.get();

    m_zArrowObject = createArrow(Z_COLOR, ArrowHead::Cone);
    m_zArrowObject->setAsRelativeTransform();
    m_minimalZArrowObject = createArrowHead(Z_COLOR, ArrowHead::Cone, 3);
    m_minimalZArrowObject->setAsRelativeTransform();
    m_zArrow = m_zArrowObject.get();
}

float Editor3DMove::findSelectedMove(Spark::Render::Camera &camera)
{
    float closest = -1;
    int currentAxis = static_cast<int>(Axis::X);
    for (auto &object : {m_xArrow, m_yArrow, m_zArrow})
    {
        float distance =
            Spark::Physics::getRayDistanceFromObject(Spark::Physics::getMouseRay(camera), object->getPhysicsObject());
        if (distance > 0 && (closest == -1 || distance < closest))
        {
            closest = distance;
            m_selectedObject = object;
            m_selectedAxis = static_cast<Axis>(currentAxis);
        }
        currentAxis++;
    }
    return closest;
}

void Editor3DMove::initializeMoveTransform(Spark::Render::Camera &camera)
{
    Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(camera);
    switch (m_selectedAxis)
    {
    case Axis::X:
        m_xArrow->getDrawable()->setColor(X_HIGHLIGHT_COLOR, true);
        m_originalMoveAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                         {{1, 0, 0}, m_xArrow->getPhysicsObject().getPosition()}, mouseRay)
                                         .x;
        break;
    case Axis::Y:
        m_yArrow->getDrawable()->setColor(Y_HIGHLIGHT_COLOR, true);
        m_originalMoveAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                         {{0, 1, 0}, m_yArrow->getPhysicsObject().getPosition()}, mouseRay)
                                         .y;
        break;
    case Axis::Z:
        m_zArrow->getDrawable()->setColor(Z_HIGHLIGHT_COLOR, true);
        m_originalMoveAxisPosition = Spark::Physics::getClosestPointToRayFromRay(
                                         {{0, 0, 1}, m_zArrow->getPhysicsObject().getPosition()}, mouseRay)
                                         .z;
        break;
    }
}

void Editor3DMove::handleMoveTransformUpdate(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit)
{
    Spark::Physics::Ray3D mouseRay = Spark::Physics::getMouseRay(camera);
    float newPosition = 0;
    switch (m_selectedAxis)
    {
    case Axis::X:
        newPosition = Spark::Physics::getClosestPointToRayFromRay(
                          {{1, 0, 0}, m_xArrow->getPhysicsObject().getPosition()}, mouseRay)
                          .x;
        objectToEdit->move({newPosition - m_originalMoveAxisPosition, 0, 0});
        m_originalMoveAxisPosition = newPosition;
        break;
    case Axis::Y:
        newPosition = Spark::Physics::getClosestPointToRayFromRay(
                          {{0, 1, 0}, m_yArrow->getPhysicsObject().getPosition()}, mouseRay)
                          .y;
        objectToEdit->move({0, newPosition - m_originalMoveAxisPosition, 0});
        m_originalMoveAxisPosition = newPosition;
        break;
    case Axis::Z:
        newPosition = Spark::Physics::getClosestPointToRayFromRay(
                          {{0, 0, 1}, m_zArrow->getPhysicsObject().getPosition()}, mouseRay)
                          .z;
        objectToEdit->move({0, 0, newPosition - m_originalMoveAxisPosition});
        m_originalMoveAxisPosition = newPosition;
        break;
    }
}

void Editor3DMove::updateObjects(Spark::Render::Camera &camera, Spark::Object3D *objectToEdit)
{
    if (m_addObjects)
    {
        m_layer.addObjectAndChilds(*m_xArrow);
        m_layer.addObjectAndChilds(*m_yArrow);
        m_layer.addObjectAndChilds(*m_zArrow);
        m_addObjects = false;
    }

    auto distanceVector = camera.getPosition() - m_xArrow->getPhysicsObject().getPosition();
    auto planeNormal = glm::normalize(camera.getFront());
    float relative = glm::abs(glm::dot(distanceVector, planeNormal));
    m_xArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_yArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_zArrow->setScale({relative * 0.05, relative * 0.05, relative * 0.05});
    m_xArrow->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_yArrow->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_zArrow->setPosition(objectToEdit->getPhysicsObject().getPosition());
}

void Editor3DMove::addTransforms(bool minimal)
{
    if (minimal != m_minimal)
    {
        m_xArrow = (minimal) ? m_minimalXArrowObject.get() : m_xArrowObject.get();
        m_yArrow = (minimal) ? m_minimalYArrowObject.get() : m_yArrowObject.get();
        m_zArrow = (minimal) ? m_minimalZArrowObject.get() : m_zArrowObject.get();

        m_minimal = minimal;
    }

    m_addObjects = true;
}

void Editor3DMove::removeTransforms()
{
    if (!m_addObjects)
    {
        m_layer.removeObjectAndChilds(*m_xArrow);
        m_layer.removeObjectAndChilds(*m_yArrow);
        m_layer.removeObjectAndChilds(*m_zArrow);
    }
    m_addObjects = false;
}

void Editor3DMove::release()
{
    m_selectedObject = nullptr;
    m_xArrow->getDrawable()->setColor(X_COLOR, true);
    m_yArrow->getDrawable()->setColor(Y_COLOR, true);
    m_zArrow->getDrawable()->setColor(Z_COLOR, true);
}
