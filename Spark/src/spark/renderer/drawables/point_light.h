#pragma once

#include "spark/objects/object.h"
#include "spark/renderer/drawables/drawable3d.h"

#include <glm/glm.hpp>

namespace Spark::Render
{
struct PointLight
{
  public:
    SPARK_API virtual ~PointLight() = default;
    SPARK_API PointLight(const PointLight &other) = default;
    SPARK_API PointLight(PointLight &&other) noexcept = default;
    SPARK_API PointLight &operator=(const PointLight &other) = default;
    SPARK_API PointLight &operator=(PointLight &&other) noexcept = default;

    SPARK_API virtual void turnOn();
    SPARK_API virtual void turnOff();
    SPARK_API virtual bool isLit();

    SPARK_API virtual glm::vec3 getColor();
    SPARK_API virtual void setColor(glm::vec3 color);

    SPARK_API virtual void move(glm::vec3 position, bool moveObject = true);
    SPARK_API virtual void setPosition(glm::vec3 position, bool moveObject = true);
    SPARK_API virtual glm::vec3 getPosition();

    SPARK_API virtual Object3D &getObject();
    SPARK_API virtual Physics::Object3D &getPhysicsObject();
    SPARK_API virtual std::shared_ptr<Render::Drawable3D> getDrawable();

  protected:
    SPARK_API PointLight(glm::vec3 position, glm::vec3 color, std::unique_ptr<Object3D> object, bool isLit);

  private:
    bool m_isLit;
    glm::vec3 m_position;
    glm::vec3 m_color;
    std::unique_ptr<Object3D> m_object;
};

SPARK_API std::shared_ptr<PointLight> createPointLight(glm::vec3 position, glm::vec3 color,
                                                       std::unique_ptr<Object3D> object, bool isLit = true);
} // namespace Spark::Render
