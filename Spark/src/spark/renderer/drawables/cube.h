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
enum class CubeType
{
    TexturedCude,
    ColoredCube
};
class Cube : public virtual Drawable
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

    SPARK_API virtual bool isHighlighted() const;
    SPARK_API virtual glm::vec3 getHighlightColor() const;
    SPARK_API virtual float getHighlightWidth() const;
    SPARK_API virtual void highlight();
    SPARK_API virtual void unhighlight();
    SPARK_API virtual void setHighlightColor(glm::vec3 color);
    SPARK_API virtual void setHighlightWidth(float width);

    virtual CubeType getType() const;

  protected:
    SPARK_API Cube(CubeType type, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0));

  private:
    CubeType m_type;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    bool m_highlight;
    glm::vec3 m_highlightColor;
    float m_highlightWidth;
};

SPARK_API std::shared_ptr<Drawable> createCube(glm::vec3 position, const Texture &texture,
                                               const Texture &specularTexture, glm::vec3 scale = glm::vec3(1.0));
SPARK_API std::shared_ptr<Drawable> createCube(glm::vec3 position, glm::vec3, glm::vec3 scale = glm::vec3(1.0));
} // namespace Spark
