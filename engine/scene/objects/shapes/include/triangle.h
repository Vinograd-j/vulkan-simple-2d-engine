#pragma once
#include "../../include/renderable.h"

class Triangle : public Renderable
{

public:

    explicit Triangle(const glm::vec3& color);

public:

    const std::vector<Vertex> GetVertices() const override { return _vertices; };

    const std::vector<uint16_t> GetIndices() const override { return _indices; };

    ShapeType GetShapeType() const override { return ShapeType::TRIANGLE; };

public:

    void CreateVertices() override;

    void CreateIndices() override;

};
