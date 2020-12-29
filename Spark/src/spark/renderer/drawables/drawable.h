#pragma once

#include "spark/core/core.h"

#include <glm/glm.hpp>

namespace Spark::Render
{
class Drawable
{
  public:
    SPARK_API virtual ~Drawable() = default;
    SPARK_API virtual glm::mat4 getTransformation() = 0;

    SPARK_API virtual bool isHighlighted() const;
    SPARK_API virtual glm::vec3 getHighlightColor() const;
    SPARK_API virtual float getHighlightWidth() const;
    SPARK_API virtual void highlight();
    SPARK_API virtual void unhighlight();
    SPARK_API virtual void setHighlightColor(glm::vec3 color);
    SPARK_API virtual void setHighlightWidth(float width);

  protected:
    Drawable();
    bool m_highlight;
    glm::vec3 m_highlightColor;
    float m_highlightWidth;
};
} // namespace Spark::Render