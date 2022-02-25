#include "camera.h"

#include "spark/core/log.h"

static const float SPEED = 25.0f;
static const float ZOOM = 45.0f;

namespace Spark::Render
{
Camera::Camera(glm::vec3 position, float maxFov, glm::vec3 up, glm::vec3 front)
    : m_position(position)
    , m_front(front)
    , m_up(up)
    , m_maxFov(maxFov)
    , m_movementSpeed(SPEED)
    , m_zoom(ZOOM)
{
    SPARK_CORE_ASSERT(maxFov >= ZOOM, "Zoom should be at least {}", ZOOM);
    m_right = glm::normalize(glm::cross(m_front, m_up));
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ)
    : m_position(glm::vec3(posX, posY, posZ))
    , m_front(glm::vec3(-1.0f, 0.0f, 0.0f))
    , m_up(glm::normalize(glm::vec3(upX, upY, upZ)))
    , m_maxFov(ZOOM * 2)
    , m_movementSpeed(SPEED)
    , m_zoom(ZOOM)
{
    m_right = glm::normalize(glm::cross(m_front, m_up));
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

void Camera::zoom(float amount)
{
    if (m_zoom >= 1.0f && m_zoom <= m_maxFov)
        m_zoom -= amount;
    if (m_zoom <= 1.0f)
        m_zoom = 1.0f;
    if (m_zoom >= m_maxFov)
        m_zoom = m_maxFov;
}
} // namespace Spark::Render
