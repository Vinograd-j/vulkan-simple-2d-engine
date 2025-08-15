#pragma once

#include <vector>
#include <glm/fwd.hpp>

#include "shape-type.h"
#include "../../../struct/vertex.h"

class Renderable
{

protected:

    std::vector<Vertex> _vertices;
    std::vector<uint16_t> _indices;

public:

    virtual const std::vector<Vertex> GetVertices() const = 0;

    virtual const std::vector<uint16_t> GetIndices() const = 0;

    virtual ShapeType GetShapeType() const = 0;

    virtual ~Renderable() = default;

protected:

    virtual void CreateVertices() = 0;

    virtual void CreateIndices() = 0;

};
