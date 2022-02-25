#include "locked_camera.h"

static const float SENSITIVITY = 0.1f;

namespace Spark::Render
{
LockedCamera::LockedCamera(glm::vec3 position, float maxFov, glm::vec3 up, float yaw, float pitch)
    : Camera(position, maxFov, up)
    , m_worldUp(up)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_sensitivity(SENSITIVITY)
{
    updateCameraVectors();
}

LockedCamera::LockedCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Camera(posX, posY, posZ, upX, upY, upZ)
    , m_worldUp({upX, upY, upZ})
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_sensitivity(SENSITIVITY)
{
    updateCameraVectors();
}

void LockedCamera::moveAngle(float yaw, float pitch, bool constrainPitch)
{
    yaw *= m_sensitivity;
    pitch *= m_sensitivity;

    m_yaw -= yaw;
    m_pitch += pitch;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void LockedCamera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    m_right = glm::normalize(
        glm::cross(m_front, m_worldUp)); // Normalize the vectors, because their length gets closer to 0 the more you
                                         // look up or down which results in slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
} // namespace Spark::Render
