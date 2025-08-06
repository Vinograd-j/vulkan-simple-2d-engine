#pragma once
#include <vector>
#include <glm/fwd.hpp>

#include "shape-type.h"
#include "../../../struct/vertex.h"

class Renderable
{

protected:

    glm::mat4 _model {};

    glm::vec4 _color {};

    std::vector<Vertex> _vertices;
    std::vector<uint32_t> _indices;

public:

    virtual const std::vector<Vertex> GetVertices() const = 0;

    virtual const std::vector<uint32_t> GetIndices() const = 0;

    virtual ShapeType GetShapeType() const = 0;

    glm::mat4 GetModelMatrix() const { return _model; }
    glm::vec4 GetColor() const { return _color; }

    virtual ~Renderable() = default;

protected:

    virtual void CreateVertices() = 0;

    virtual void CreateIndices() = 0;

};
