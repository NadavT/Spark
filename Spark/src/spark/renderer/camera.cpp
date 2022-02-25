#include "camera.h"

#include "spark/core/log.h"

static const float SPEED = 25.0f;
static const float SENSITIVITY = 0.1f;
static const float ZOOM = 45.0f;

namespace Spark::Render
{
Camera::Camera(glm::vec3 position, float maxFov, glm::vec3 up, float yaw, float pitch)
    : m_position(position)
    , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_worldUp(up)
    , m_maxFov(maxFov)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_movementSpeed(SPEED)
    , m_sensitivity(SENSITIVITY)
    , m_zoom(ZOOM)
{
    SPARK_CORE_ASSERT(maxFov >= ZOOM, "Zoom should be at least {}", ZOOM);
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : m_position(glm::vec3(posX, posY, posZ))
    , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_worldUp(glm::vec3(upX, upY, upZ))
    , m_maxFov(ZOOM * 2)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_movementSpeed(SPEED)
    , m_sensitivity(SENSITIVITY)
    , m_zoom(ZOOM)
{
    updateCameraVectors();
}

float Camera::getMaxFov() const
{
    return m_maxFov;
}

void Camera::setMaxFov(float maxFov)
{
    m_maxFov = maxFov;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

float Camera::getZoom() const
{
    return m_zoom;
}

float Camera::getZoomRadians() const
{
    return glm::radians(m_zoom);
}

glm::vec3 Camera::getPosition() const
{
    return m_position;
}

glm::vec3 Camera::getFront() const
{
    return m_front;
}

glm::vec3 Camera::getUp() const
{
    return m_up;
}

glm::vec3 Camera::getRight() const
{
    return m_right;
}

void Camera::setPosition(const glm::vec3 &newPosition)
{
    m_position = newPosition;
}

void Camera::setFront(const glm::vec3 &newFront)
{
    m_front = glm::normalize(newFront);
    m_pitch = glm::degrees(asin(m_front.z));
    m_yaw = glm::degrees(acos(m_front.x / cos(glm::radians(m_pitch))));
    if (sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)) - m_front.y >= 100 * glm::epsilon<float>())
    {
        m_yaw = -m_yaw;
    }
    // updateCameraVectors();
    SPARK_CORE_TRACE("yaw: {}, pitch: {}", m_yaw, m_pitch);
}

void Camera::moveDirection(CameraDirection direction, Time deltaTime)
{
    float velocity = m_movementSpeed * static_cast<float>(deltaTime.GetSeconds());
    moveDirection(direction, velocity);
}

void Camera::moveDirection(CameraDirection direction, float amount)
{
    if (direction == CameraDirection::FORWARD)
        m_position += m_front * amount;
    if (direction == CameraDirection::BACKWARD)
        m_position -= m_front * amount;
    if (direction == CameraDirection::LEFT)
        m_position -= m_right * amount;
    if (direction == CameraDirection::RIGHT)
        m_position += m_right * amount;
    if (direction == CameraDirection::UP)
        m_position += m_up * amount;
    if (direction == CameraDirection::DOWN)
        m_position -= m_up * amount;
}

void Camera::moveAngle(float yaw, float pitch, bool constrainPitch)
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

void Camera::zoom(float amount)
{
    if (m_zoom >= 1.0f && m_zoom <= m_maxFov)
        m_zoom -= amount;
    if (m_zoom <= 1.0f)
        m_zoom = 1.0f;
    if (m_zoom >= m_maxFov)
        m_zoom = m_maxFov;
}

void Camera::updateCameraVectors()
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
