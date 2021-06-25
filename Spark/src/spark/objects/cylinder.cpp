#include "cylinder.h"

#include "spark/physics/cylinder.h"
#include "spark/renderer/drawables/cylinder.h"

namespace Spark
{
Cylinder::Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, glm::vec3 color, int sectors,
                   int stacks)
    : Object3D(Render::createCylinder(position, color, baseRadius, topRadius, height, sectors, stacks),
               std::make_unique<Physics::Cylinder>(position, baseRadius, topRadius, height))
{
}

Cylinder::Cylinder(glm::vec3 position, float baseRadius, float topRadius, float height, const Texture &texture,
                   const Texture &specularTexture, int sectors, int stacks)
    : Object3D(
          Render::createCylinder(position, texture, specularTexture, baseRadius, topRadius, height, sectors, stacks),
          std::make_unique<Physics::Cylinder>(position, baseRadius, topRadius, height))
{
}

std::unique_ptr<Object3D> createCylinder(glm::vec3 position, float baseRadius, float topRadius, float height,
                                         glm::vec3 color, int sectors, int stacks)
{
    return std::make_unique<Cylinder>(position, baseRadius, topRadius, height, color, sectors, stacks);
}

std::unique_ptr<Object3D> createCylinder(glm::vec3 position, float baseRadius, float topRadius, float height,
                                         const Texture &texture, const Texture &specularTexture, int sectors,
                                         int stacks)
{
    return std::make_unique<Cylinder>(position, baseRadius, topRadius, height, texture, specularTexture, sectors,
                                      stacks);
}

} // namespace Spark