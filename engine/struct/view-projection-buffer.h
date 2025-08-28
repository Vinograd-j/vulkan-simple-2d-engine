#pragma once

#include <glm/glm.hpp>

struct ViewProjectionBuffer
{
    alignas(16) glm::mat4 _view;
    alignas(16) glm::mat4 _proj;
};