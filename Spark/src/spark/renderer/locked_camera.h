#ifndef SPARK_LOCKED_CAMERA_H
#define SPARK_LOCKED_CAMERA_H

#include "camera.h"

namespace Spark::Render
{

const float YAW = 180.0f;
const float PITCH = 0.0f;
class LockedCamera : public Camera
{
  public:
    SPARK_API LockedCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float maxFov = 45.0f,
                           glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f), float yaw = YAW, float pitch = PITCH);
    SPARK_API LockedCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    SPARK_API void moveAngle(float yaw, float pitch, bool constrainPitch = true);

  private:
    void updateCameraVectors();

  private:
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;

    float m_sensitivity;
};
} // namespace Spark::Render

#endif /* SPARK_LOCKED_CAMERA_H */
