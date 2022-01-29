#include "editor_3d_scale.h"

static const glm::vec3 X_COLOR = {0.5f, 0, 0};
static const glm::vec3 X_HIGHLIGHT_COLOR = {1, 0, 0};
static const glm::vec3 Y_COLOR = {0, 0.5f, 0};
static const glm::vec3 Y_HIGHLIGHT_COLOR = {0, 1, 0};
static const glm::vec3 Z_COLOR = {0, 0, 0.5f};
static const glm::vec3 Z_HIGHLIGHT_COLOR = {0, 0, 1};

Editor3DScale::Editor3DScale(Spark::Layer3D &layer)
    : m_layer(layer)
    , m_xArrow(nullptr)
    , m_yArrow(nullptr)
    , m_zArrow(nullptr)
    , m_selectedObject(nullptr)
    , m_selectedAxis(Axis::X)
    , m_originalScaleAxisPosition(0)
{
    m_xArrow = createArrow(X_COLOR);
    m_xArrow->rotate(glm::radians(90.0f), {0, 1, 0});
    m_xArrow->setAsRelativeTransform();

    m_yArrow = createArrow(Y_COLOR);
    m_yArrow->rotate(glm::radians(270.0f), {1, 0, 0});
    m_yArrow->setAsRelativeTransform();

    m_zArrow = createArrow(Z_COLOR);
    m_zArrow->setAsRelativeTransform();
}

float Editor3DScale::findSelectedScale(Spark::Render::Camera &camera)
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
    m_xArrow->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_yArrow->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_zArrow->setPosition(objectToEdit->getPhysicsObject().getPosition());
    m_xArrow->setRotation(objectToEdit->getPhysicsObject().getRotation());
    m_yArrow->setRotation(objectToEdit->getPhysicsObject().getRotation());
    m_zArrow->setRotation(objectToEdit->getPhysicsObject().getRotation());
}

void Editor3DScale::addTransforms()
{
    m_layer.addObjectAndChilds(*m_xArrow);
    m_layer.addObjectAndChilds(*m_yArrow);
    m_layer.addObjectAndChilds(*m_zArrow);
}

void Editor3DScale::removeTransforms()
{
    m_layer.removeObjectAndChilds(*m_xArrow);
    m_layer.removeObjectAndChilds(*m_yArrow);
    m_layer.removeObjectAndChilds(*m_zArrow);
}

void Editor3DScale::release()
{
    m_selectedObject = nullptr;
    m_xArrow->getDrawable()->setColor(X_COLOR, true);
    m_yArrow->getDrawable()->setColor(Y_COLOR, true);
    m_zArrow->getDrawable()->setColor(Z_COLOR, true);
}

std::shared_ptr<Spark::Object3D> Editor3DScale::createArrow(glm::vec3 color)
{
    std::shared_ptr<Spark::Object3D> arrowBody = Spark::createCylinder(glm::vec3(0, 0, 0), 0.2f, 0.2f, 2, color);
    std::shared_ptr<Spark::Object3D> arrowHead = Spark::createBox(glm::vec3(0, 0, 1.25f), 0.5f, 0.5f, 0.5f, color);
    arrowBody->addChild(arrowHead);
    arrowHead->getDrawable()->setCalculateLight(false);
    auto boxBound = std::make_unique<Spark::Physics::Box>(glm::vec3(0, 0, 0.25), 1.0f, 1.0f, 2.5f);
    arrowBody->setPhysicsObject(std::move(boxBound));
    arrowBody->move({0, 0, 1});
    arrowBody->setAsRelativeTransform();
    arrowBody->getDrawable()->setCalculateLight(false);
    return arrowBody;
}
