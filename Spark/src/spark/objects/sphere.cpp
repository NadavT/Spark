#include "sphere.h"

#include "spark/physics/sphere.h"
#include "spark/renderer/drawables/sphere.h"

namespace Spark
{
SPARK_API Sphere::Sphere(glm::vec3 position, float radius, glm::vec3 color, int sectors, int stacks)
    : Object3D(Render::createSphere(position, color, sectors, stacks, glm::vec3(radius * 2, radius * 2, radius * 2)),
               std::make_unique<Physics::Sphere>(position, radius))
{
}

SPARK_API Sphere::Sphere(glm::vec3 position, float radius, const Texture &texture, const Texture &specularTexture,
                         int sectors, int stacks)
    : Object3D(Render::createSphere(position, texture, specularTexture, sectors, stacks,
                                    glm::vec3(radius * 2, radius * 2, radius * 2)),
               std::make_unique<Physics::Sphere>(position, radius))
{
}
} // namespace Spark