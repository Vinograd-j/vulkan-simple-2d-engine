#include "../include/triangle.h"

Triangle::Triangle(const glm::vec3& color)
{
    _color = color;
    Triangle::CreateVertices();
    Triangle::CreateIndices();
}

void Triangle::CreateVertices()
{
    const std::vector<Vertex> vertices = {
        {{0.0f, -0.2f}, {1.0f, 0.0f, 0.0f}},
        {{0.2f, 0.2f}, {0.0f, 1.0f, 0.0f}},
        {{-0.2f, 0.2f}, {0.0f, 0.0f, 1.0f}}
    };

    _vertices = vertices;
}

void Triangle::CreateIndices()
{
    _indices = {0, 1, 2};
}