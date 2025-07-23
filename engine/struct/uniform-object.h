#pragma once

#include "glm/glm.hpp"

struct UniformObject
{
    alignas(16) glm::mat4 _model;
};