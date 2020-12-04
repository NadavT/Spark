#pragma once

#include "spark/renderer/drawable.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

namespace Spark
{
struct PointLight
{
  public:
    SPARK_API virtual ~PointLight() = default;
    SPARK_API PointLight(const PointLight &other) = default;
    SPARK_API PointLight(PointLight &&other) noexcept = default;
    SPARK_API PointLight &operator=(const PointLight &other) = default;
    SPARK_API PointLight &operator=(PointLight &&other) noexcept = default;

    SPARK_API virtual void turnOn()
    {
        m_isLit = true;
    };
    SPARK_API virtual void turnOff()
    {
        m_isLit = false;
    };
    SPARK_API virtual bool isLit()
    {
        return m_isLit;
    };

    glm::vec3 position;
    glm::vec3 color;
    std::shared_ptr<Drawable> drawable;

  protected:
    SPARK_API PointLight(glm::vec3 position, glm::vec3 color, std::shared_ptr<Drawable> &drawable, bool isLit)
        : position(position)
        , color(color)
        , drawable(drawable)
        , m_isLit(isLit){};

  private:
    bool m_isLit;
};

SPARK_API std::shared_ptr<PointLight> createPointLight(glm::vec3 position, glm::vec3 color,
                                                       std::shared_ptr<Drawable> &drawable, bool isLit = true);
} // namespace Spark
