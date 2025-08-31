#pragma once

#include <glm/glm.hpp>

enum CameraKey
{
    W, S, A, D
};

class Camera
{
private:

    glm::vec2 _position {};
    glm::vec2 _velocity {};

public:

    glm::mat4 GetViewMatrix() const;

    void Process(const CameraKey& key, bool pressed);

    void Update(float deltaTime);

};
