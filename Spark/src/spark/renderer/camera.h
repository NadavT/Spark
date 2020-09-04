#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "spark/core/time.h"

namespace Spark
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

    const float YAW = 180.0f;
    const float PITCH = 0.0f;
    const float SPEED = 25.0f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

    class Camera
    {
    public:
        SPARK_API Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f), float yaw = YAW, float pitch = PITCH);
        SPARK_API Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

        glm::mat4 getViewMatrix();
        float getZoom();

        SPARK_API void moveDirection(CameraDirection direction, Time deltaTime);
        SPARK_API void moveAngle(float yaw, float pitch, bool constrainPitch = true);
        SPARK_API void zoom(float amount);        

    private:
        void updateCameraVectors();

    private:
        glm::vec3 m_position;
        glm::vec3 m_front;
        glm::vec3 m_up;
        glm::vec3 m_right;
        glm::vec3 m_worldUp;

        float m_yaw;
        float m_pitch;

        float m_movementSpeed;
        float m_sensitivity;
        float m_zoom;
    };
}
