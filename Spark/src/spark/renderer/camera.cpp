#include "camera.h"

namespace Spark
{
    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : m_position(position)
        , m_worldUp(up)
        , m_yaw(yaw)
        , m_pitch(pitch)
        , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_movementSpeed(SPEED)
        , m_sensitivity(SENSITIVITY)
        , m_zoom(ZOOM)
    {
        updateCameraVectors();
    }

    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : m_position(glm::vec3(posX, posY, posZ))
        , m_worldUp(glm::vec3(upX, upY, upZ))
        , m_yaw(yaw)
        , m_pitch(pitch)
        , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_movementSpeed(SPEED)
        , m_sensitivity(SENSITIVITY)
        , m_zoom(ZOOM)
    {
        updateCameraVectors();
    }
    
    glm::mat4 Camera::getViewMatrix() 
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }
    
    float Camera::getZoom() 
    {
        return m_zoom;
    }
    
    void Camera::moveDirection(CameraDirection direction, Time deltaTime) 
    {
        float velocity = m_movementSpeed * static_cast<float>(deltaTime.GetSeconds());
        if (direction == CameraDirection::FORWARD)
            m_position += m_front * velocity;
        if (direction == CameraDirection::BACKWARD)
            m_position -= m_front * velocity;
        if (direction == CameraDirection::LEFT)
            m_position -= m_right * velocity;
        if (direction == CameraDirection::RIGHT)
            m_position += m_right * velocity;
        if (direction == CameraDirection::UP)
            m_position += m_up * velocity;
        if (direction == CameraDirection::DOWN)
            m_position -= m_up * velocity;   
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
        if (m_zoom >= 1.0f && m_zoom <= 45.0f)
            m_zoom -= amount;
        if (m_zoom <= 1.0f)
            m_zoom = 1.0f;
        if (m_zoom >= 45.0f)
            m_zoom = 45.0f;
    }
    
    void Camera::updateCameraVectors() 
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_pitch));
        m_front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
}
