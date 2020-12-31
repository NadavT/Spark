#pragma once

#include "object3d_bounding.h"

#include <glm/glm.hpp>

namespace Spark::Physics
{
class BoxBounding : public Object3DBounding
{
  public:
    BoxBounding(glm::vec3 position, float width, float length, float height);
    virtual ~BoxBounding() = default;

    inline glm::vec3 getPosition() const
    {
        return m_position;
    };
    inline float getWidth() const
    {
        return m_width;
    };
    inline float getLength() const
    {
        return m_length;
    };
    inline float getHeight() const
    {
        return m_height;
    };

  private:
    glm::vec3 m_position;
    float m_width;
    float m_length;
    float m_height;
};

} // namespace Spark::Physics
