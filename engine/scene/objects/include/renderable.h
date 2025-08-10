#pragma once
#include <vector>
#include <glm/fwd.hpp>

#include "shape-type.h"
#include "../../../struct/vertex.h"

class Renderable
{

protected:

    glm::mat4 _model {};

    glm::vec3 _color {};

    std::vector<Vertex> _vertices;
    std::vector<uint16_t> _indices;

public:

    virtual const std::vector<Vertex> GetVertices() const = 0;

    virtual const std::vector<uint16_t> GetIndices() const = 0;

    virtual ShapeType GetShapeType() const = 0;

    glm::mat4 GetModelMatrix() const { return _model; }
    void UpdateModel(const glm::mat4& model) { _model = model; }

    glm::vec3 GetColor() const { return _color; }

    virtual ~Renderable() = default;

protected:

    virtual void CreateVertices() = 0;

    virtual void CreateIndices() = 0;

};
