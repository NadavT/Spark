#include "cylinder.h"

#include "spark/physics/cylinder.h"
#include "spark/renderer/drawables/cylinder.h"

namespace Spark
{
Cylinder::Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, glm::vec3 color)
    : Object3D(Render::createCylinder(position, color, baseRadius, topRadius, height),
               std::make_unique<Physics::Cylinder>(position, baseRadius, topRadius, height))
{
}

Cylinder::Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, const Texture &texture,
                   const Texture &specularTexture)
    : Object3D(Render::createCylinder(position, texture, specularTexture, baseRadius, topRadius, height),
               std::make_unique<Physics::Cylinder>(position, baseRadius, topRadius, height))
{
}

std::unique_ptr<Object3D> createCylinder(glm::vec3 position, float baseRadius, float topRadius, float height,
                                         glm::vec3 color)
{
    return std::make_unique<Cylinder>(position, baseRadius, topRadius, height, color);
}

std::unique_ptr<Object3D> createCylinder(glm::vec3 position, float baseRadius, float topRadius, float height,
                                         const Texture &texture, const Texture &specularTexture)
{
    return std::make_unique<Cylinder>(position, baseRadius, topRadius, height, texture, specularTexture);
}

} // namespace Spark