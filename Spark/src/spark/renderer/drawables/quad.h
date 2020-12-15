#pragma once

#include "spark/core/core.h"

#include "spark/renderer/drawable.h"
#include "spark/resource/texture.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <memory>

namespace Spark
{
class Quad : public virtual Drawable
{
  public:
    SPARK_API virtual ~Quad() = default;
    SPARK_API Quad(const Quad &other) = default;
    SPARK_API Quad(Quad &&other) noexcept = default;
    SPARK_API Quad &operator=(const Quad &other) = default;
    SPARK_API Quad &operator=(Quad &&other) noexcept = default;

    SPARK_API virtual void move(glm::vec2 position);
    SPARK_API virtual void scale(glm::vec2 scale);
    SPARK_API virtual glm::mat3 getTransformation();

  protected:
    SPARK_API Quad(glm::vec2 position, glm::vec2 scale = glm::vec2(1.0));

  private:
    glm::mat3 m_transformation;
};

SPARK_API std::shared_ptr<Drawable> createQuad(glm::vec2 position, const Texture &texture,
                                               glm::vec2 scale = glm::vec2(1.0));
} // namespace Spark
