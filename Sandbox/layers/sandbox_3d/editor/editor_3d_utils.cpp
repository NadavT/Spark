#include "editor_3d_utils.h"

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

std::shared_ptr<Spark::Object3D> createArrowHead(glm::vec3 color, ArrowHead type, float length, float headSize)
{
    std::shared_ptr<Spark::Object3D> arrowHead =
        (type == ArrowHead::Cone)
            ? Spark::createCylinder(glm::vec3(0, 0, length / 2 + headSize / 2), headSize, 0, headSize, color)
            : Spark::createBox(glm::vec3(0, 0, length / 2 + headSize / 2), headSize, headSize, headSize, color);
    auto boxBound =
        std::make_unique<Spark::Physics::Box>(glm::vec3(0, 0, length / 2 + headSize / 2), headSize, headSize, headSize);
    arrowHead->setPhysicsObject(std::move(boxBound));
    arrowHead->move({0, 0, length / 2});
    arrowHead->setAsRelativeTransform();
    arrowHead->getDrawable()->setCalculateLight(false);
    return arrowHead;
}

std::shared_ptr<Spark::Object3D> createArrow(glm::vec3 color, ArrowHead type, float length, float headSize,
                                             float bodyWidth)
{
    std::shared_ptr<Spark::Object3D> arrowBody =
        Spark::createCylinder(glm::vec3(0, 0, 0), bodyWidth, bodyWidth, length, color);
    std::shared_ptr<Spark::Object3D> arrowHead =
        (type == ArrowHead::Cone)
            ? Spark::createCylinder(glm::vec3(0, 0, length / 2 + headSize / 2), headSize, 0, headSize, color)
            : Spark::createBox(glm::vec3(0, 0, length / 2 + headSize / 2), headSize, headSize, headSize, color);
    arrowBody->addChild(arrowHead);
    arrowHead->getDrawable()->setCalculateLight(false);
    auto boxBound = std::make_unique<Spark::Physics::Box>(glm::vec3(0, 0, headSize / 2), headSize * 2, headSize * 2,
                                                          length + headSize);
    arrowBody->setPhysicsObject(std::move(boxBound));
    arrowBody->move({0, 0, length / 2});
    arrowBody->setAsRelativeTransform();
    arrowBody->getDrawable()->setCalculateLight(false);
    return arrowBody;
}

std::shared_ptr<Spark::Object3D> createRing(glm::vec3 color)
{
    std::shared_ptr<Spark::Object3D> ring = Spark::createPipe(buildCircle(2.5f, 64), 0.05f, true, color);
    auto ringBound = std::make_unique<Spark::Physics::Pipe>(buildCircle(2.5f, 64), 0.15f, true);
    ring->setPhysicsObject(std::move(ringBound));
    ring->getDrawable()->setCalculateLight(false);
    return ring;
}
