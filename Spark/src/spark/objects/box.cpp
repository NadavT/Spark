#include "box.h"

#include "spark/physics/box.h"
#include "spark/renderer/drawables/box.h"

namespace Spark
{
Box::Box(glm::vec3 position, float width, float length, float height, glm::vec3 color)
    : Object3D(Render::createBox(position, color, glm::vec3(width, length, height)),
               std::make_unique<Physics::Box>(position, width, length, height))
{
}

Box::Box(glm::vec3 position, float width, float length, float height, const Texture &texture,
         const Texture &specularTexture)
    : Object3D(Render::createBox(position, texture, specularTexture, glm::vec3(width, length, height)),
               std::make_unique<Physics::Box>(position, width, length, height))
{
}

std::unique_ptr<Object3D> createBox(glm::vec3 position, float width, float length, float height, glm::vec3 color)
{
    return std::make_unique<Box>(position, width, length, height, color);
}

std::unique_ptr<Object3D> createBox(glm::vec3 position, float width, float length, float height, const Texture &texture,
                                    const Texture &specularTexture)
{
    return std::make_unique<Box>(position, width, length, height, texture, specularTexture);
}

} // namespace Spark