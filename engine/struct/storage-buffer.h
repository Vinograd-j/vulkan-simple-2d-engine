#pragma once

#include "glm/glm.hpp"

struct alignas(16) StorageBufferObject
{
    glm::mat4 _model;
    glm::vec3 _color;
};