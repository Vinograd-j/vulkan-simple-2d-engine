#include "../include/circle.h"

#include <glm/ext/scalar_constants.hpp>

Circle::Circle(float radius, int segmentCount) : _radius(radius), _segmentCount(segmentCount)
{
    GenerateCircle();
}

void Circle::GenerateCircle()
{
    CreateVertices();
    CreateIndices();
}

void Circle::CreateVertices()
{
    std::vector<Vertex> vertices;

    vertices.push_back({ glm::vec2(0.0f, 0.0f), {1.0f, 0.0f, 0.0f} });

    for (int i = 0; i <= _segmentCount; ++i)
    {
        float angle = 2.0f * glm::pi<float>() * i / _segmentCount;
        float x = _radius * cos(angle);
        float y = _radius * sin(angle);
        vertices.push_back({ glm::vec2(x, y),  {0.0f, 1.0f, 0.0f} });
    }

    _vertices = vertices;
}

void Circle::CreateIndices()
{
    std::vector<uint16_t> indices;

    for (int i = 1; i <= _segmentCount; ++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    _indices = indices;
}