#ifndef SPARK_RENDER_DRAWABLE_H
#define SPARK_RENDER_DRAWABLE_H

#include "spark/core/core.h"

#include <glm/glm.hpp>

#include <vector>

namespace Spark::Render
{
class Drawable
{
  protected:
    static inline const float defaultHighlightWidth = 0.05f;

  public:
    SPARK_API virtual ~Drawable() = default;
    SPARK_API virtual glm::mat4 getTransformation() const = 0;

    SPARK_API virtual bool isHighlighted() const;
    SPARK_API virtual glm::vec3 getHighlightColor() const;
    SPARK_API virtual float getHighlightWidth() const;
    SPARK_API virtual void highlight();
    SPARK_API virtual void unhighlight();
    SPARK_API virtual void setHighlightColor(glm::vec3 color);
    SPARK_API virtual void setHighlightWidth(float width);

    virtual void setParent(Drawable *parent);
    virtual void removeParent();

    virtual Drawable *getParent() const;
    virtual const std::vector<Drawable *> &getChilds() const;

  protected:
    Drawable();
    Drawable(const Drawable &other) = default;
    Drawable(Drawable &&other) noexcept = default;
    Drawable &operator=(const Drawable &other) = default;
    Drawable &operator=(Drawable &&other) noexcept = default;
    virtual void addChild(Drawable *child);
    virtual void removeChild(Drawable *child);
    virtual void clearChilds();

  protected:
    bool m_highlight;
    glm::vec3 m_highlightColor;
    float m_highlightWidth;
    Drawable *m_parent;
    std::vector<Drawable *> m_childs;
};
} // namespace Spark::Render

#endif /* SPARK_RENDER_DRAWABLE_H */
