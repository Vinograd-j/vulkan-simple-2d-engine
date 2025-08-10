#pragma once

#include "glm/glm.hpp"

struct alignas(16) ObjectData
{
    glm::mat4 _model;
    glm::vec3 _color;

    int _type;
    int _indexOffset;
    int _indexCount;
};