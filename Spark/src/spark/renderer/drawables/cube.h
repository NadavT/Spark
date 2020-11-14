#pragma once

#include "spark/core/core.h"

#include "spark/renderer/drawable.h"
#include "spark/resource/texture.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace Spark
{
class Cube : public Drawable
{
  public:
    SPARK_API virtual ~Cube() = default;
    SPARK_API Cube(const Cube &other) = default;
    SPARK_API Cube(Cube &&other) noexcept = default;
    SPARK_API Cube &operator=(const Cube &other) = default;
    SPARK_API Cube &operator=(Cube &&other) noexcept = default;

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual glm::mat4 getTransformation();

  protected:
    SPARK_API Cube(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0));

  private:
    glm::mat4 m_transformation;
};

SPARK_API std::shared_ptr<Drawable> createCube(glm::vec3 position, const Texture &texture,
                                               const Texture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
} // namespace Spark
