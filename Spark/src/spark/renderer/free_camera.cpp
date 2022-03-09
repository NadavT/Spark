#include "free_camera.h"

namespace Spark::Render
{
FreeCamera::FreeCamera(glm::vec3 position, float maxFov, glm::vec3 up, glm::vec3 front)
    : Camera(position, maxFov, up, front)
{
}

FreeCamera::FreeCamera(float posX, float posY, float posZ, float upX, float upY, float upZ)
    : Camera(posX, posY, posZ, upX, upY, upZ)
{
}

void FreeCamera::setFrontUp(const glm::vec3 &newFront, const glm::vec3 &newUp)
{
    m_front = glm::normalize(newFront);
    m_up = glm::normalize(newUp);
    m_right = glm::normalize(glm::cross(m_front, m_up));
}

void FreeCamera::setFrontRight(const glm::vec3 &newFront, const glm::vec3 &newRight)
{
    m_front = glm::normalize(newFront);
    m_right = glm::normalize(newRight);
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

} // namespace Spark::Render
