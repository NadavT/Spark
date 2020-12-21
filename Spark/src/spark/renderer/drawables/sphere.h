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
class Sphere : public virtual Drawable
{
  public:
    SPARK_API virtual ~Sphere() = default;
    SPARK_API Sphere(const Sphere &other) = default;
    SPARK_API Sphere(Sphere &&other) noexcept = default;
    SPARK_API Sphere &operator=(const Sphere &other) = default;
    SPARK_API Sphere &operator=(Sphere &&other) noexcept = default;

    SPARK_API virtual void move(glm::vec3 position);
    SPARK_API virtual void scale(glm::vec3 scale);
    SPARK_API virtual glm::mat4 getTransformation();

  protected:
    SPARK_API Sphere(glm::vec3 position, int sectors, int stacks, glm::vec3 scale = glm::vec3(1.0));

  protected:
    int m_sectors;
    int m_stacks;

  private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
};

SPARK_API std::shared_ptr<Drawable> createSphere(glm::vec3 position, const Texture &texture,
                                                 const Texture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
SPARK_API std::shared_ptr<Drawable> createSphere(glm::vec3 position, glm::vec3 color, int sectors = 36, int stacks = 18,
                                                 glm::vec3 scale = glm::vec3(1.0));
} // namespace Spark
