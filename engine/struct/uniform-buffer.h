#pragma once

#include <glm/glm.hpp>

struct UniformObject
{
    alignas(16) glm::mat4 _model;
    alignas(16) glm::vec3 _color;
};
