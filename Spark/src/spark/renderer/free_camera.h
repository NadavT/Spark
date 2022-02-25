#ifndef SPARK_FREE_CAMERA_H
#define SPARK_FREE_CAMERA_H

#include "camera.h"

namespace Spark::Render
{
class FreeCamera : public Camera
{
  public:
    SPARK_API FreeCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float maxFov = 45.0f,
                         glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 front = glm::vec3(-1.0f, 0.0f, 0.0f));
    SPARK_API FreeCamera(float posX, float posY, float posZ, float upX, float upY, float upZ);

    SPARK_API void setFrontUp(const glm::vec3 &newFront, const glm::vec3 &newUp);
    SPARK_API void setFrontRight(const glm::vec3 &newFront, const glm::vec3 &newRight);
};
} // namespace Spark::Render

#endif /* SPARK_FREE_CAMERA_H */
