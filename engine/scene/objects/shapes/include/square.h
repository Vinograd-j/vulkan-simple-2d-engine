#pragma once

#include "../../include/renderable.h"

class Square : public Renderable
{

public:

    explicit Square();

public:

    const std::vector<Vertex> GetVertices() const override { return _vertices; };

    const std::vector<uint16_t> GetIndices() const override { return _indices; };

    ShapeType GetShapeType() const override { return ShapeType::SQUARE; };

public:

    void CreateVertices() override;

    void CreateIndices() override;

};