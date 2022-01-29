#include "editor_utils.h"

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

std::shared_ptr<Spark::Object3D> createRing(glm::vec3 color)
{
    std::shared_ptr<Spark::Object3D> ring = Spark::createPipe(buildCircle(2.5f, 64), 0.05f, true, color);
    auto ringBound = std::make_unique<Spark::Physics::Pipe>(buildCircle(2.5f, 64), 0.15f, true);
    ring->setPhysicsObject(std::move(ringBound));
    ring->getDrawable()->setCalculateLight(false);
    return ring;
}
