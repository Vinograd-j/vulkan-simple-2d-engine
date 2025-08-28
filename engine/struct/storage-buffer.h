#pragma once

#include "glm/glm.hpp"

struct StorageBufferObject
{
     alignas(16) uint32_t _objectId;
     alignas(16) glm::mat4 _model;
     alignas(16) glm::vec3 _color;
};