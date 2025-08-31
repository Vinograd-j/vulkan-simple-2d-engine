#include "../include/camera.h"

#include <glm/ext/matrix_transform.hpp>

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::translate(glm::mat4(1.0), glm::vec3(-_position, 0.0f));
}

void Camera::Update(float deltaTime)
{
    _position += glm::vec2(glm::vec3(_velocity * deltaTime * 0.5f, 0.f));
}

void Camera::Process(const CameraKey& key, const bool pressed)
{
    switch (key)
    {
        case W: _velocity.y = pressed ?  1 : 0; break;
        case S: _velocity.y = pressed ? -1 : 0; break;
        case D: _velocity.x = pressed ?  1 : 0; break;
        case A: _velocity.x = pressed ? -1 : 0; break;
        default: ;
    }
}
