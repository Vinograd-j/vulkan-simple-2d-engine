#pragma once

#include "../../include/renderable.h"

class Circle : public Renderable
{

private:

    float _radius;

    int _segmentCount;

public:

    explicit Circle(float radius, int segmentCount, const glm::vec3& color);

    ~Circle() override = default;

private:

    void GenerateCircle();

public:

    const std::vector<Vertex> GetVertices() const override { return _vertices; }

    const std::vector<uint16_t> GetIndices() const override { return _indices; }

    ShapeType GetShapeType() const override { return ShapeType::CIRCLE; }

protected:

    void CreateVertices() override;

    void CreateIndices() override;

};
