#include "pipe.h"

#include "spark/physics/box.h"
#include "spark/renderer/drawables/pipe.h"
#include "spark/utils/math.h"

namespace Spark
{
Pipe::Pipe(std::vector<glm::vec3> positions, float radius, bool closed, glm::vec3 color, int sectors)
    : Object3D(Render::createPipe(positions, radius, closed, color, sectors),
               std::make_unique<Physics::Box>(mid_range(positions), radius, radius, radius))
{
}

Pipe::Pipe(std::vector<glm::vec3> positions, float radius, bool closed, const Texture &texture,
           const Texture &specularTexture, int sectors)
    : Object3D(Render::createPipe(positions, radius, closed, texture, specularTexture, sectors),
               std::make_unique<Physics::Box>(mid_range(positions), radius, radius, radius))
{
}

std::unique_ptr<Object3D> createPipe(std::vector<glm::vec3> positions, float radius, bool closed, glm::vec3 color,
                                     int sectors)
{
    return std::make_unique<Pipe>(positions, radius, closed, color, sectors);
}

std::unique_ptr<Object3D> createPipe(std::vector<glm::vec3> positions, float radius, bool closed,
                                     const Texture &texture, const Texture &specularTexture, int sectors)
{
    return std::make_unique<Pipe>(positions, radius, closed, texture, specularTexture, sectors);
}
} // namespace Spark