#ifndef SPARK_RENDER_CAMERA_H
#define SPARK_RENDER_CAMERA_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "spark/core/time.h"

namespace Spark::Render
{
enum class CameraDirection
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera
{
  public:
    SPARK_API Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float maxFov = 45.0f,
                     glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 front = glm::vec3(-1.0f, 0.0f, 0.0f));
    SPARK_API Camera(float posX, float posY, float posZ, float upX, float upY, float upZ);

    SPARK_API float getMaxFov() const;
    SPARK_API void setMaxFov(float maxFov);

    glm::mat4 getViewMatrix() const;
    SPARK_API float getZoom() const;
    SPARK_API float getZoomRadians() const;
    SPARK_API glm::vec3 getPosition() const;
    SPARK_API glm::vec3 getFront() const;
    SPARK_API glm::vec3 getUp() const;
    SPARK_API glm::vec3 getRight() const;

    SPARK_API void setPosition(const glm::vec3 &newPosition);

    SPARK_API void moveDirection(CameraDirection direction, Time deltaTime);
    SPARK_API void moveDirection(CameraDirection direction, float amount);
    SPARK_API void zoom(float amount);

  protected:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    float m_maxFov;

    float m_movementSpeed;
    float m_zoom;
};
} // namespace Spark::Render

#endif /* SPARK_RENDER_CAMERA_H */
