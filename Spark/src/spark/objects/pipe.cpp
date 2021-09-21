#include "pipe.h"

#include "spark/physics/sphere.h"
#include "spark/renderer/drawables/pipe.h"
#include "spark/utils/math.h"

namespace Spark
{
Pipe::Pipe(std::vector<glm::vec3> positions, float radius, glm::vec3 color, int sectors)
    : Object3D(Render::createPipe(positions, radius, color, sectors),
               std::make_unique<Physics::Sphere>(mid_range(positions), radius))
{
}

Pipe::Pipe(std::vector<glm::vec3> positions, float radius, const Texture &texture, const Texture &specularTexture,
           int sectors)
    : Object3D(Render::createPipe(positions, radius, texture, specularTexture, sectors),
               std::make_unique<Physics::Sphere>(mid_range(positions), radius))
{
}

std::unique_ptr<Object3D> createPipe(std::vector<glm::vec3> positions, float radius, glm::vec3 color, int sectors)
{
    return std::make_unique<Pipe>(positions, radius, color, sectors);
}

std::unique_ptr<Object3D> createPipe(std::vector<glm::vec3> positions, float radius, const Texture &texture,
                                     const Texture &specularTexture, int sectors)
{
    return std::make_unique<Pipe>(positions, radius, texture, specularTexture, sectors);
}
} // namespace Spark