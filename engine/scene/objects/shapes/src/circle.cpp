#include "../include/circle.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>

Circle::Circle(float radius, int segmentCount, const glm::vec3& color) : _radius(radius), _segmentCount(segmentCount)
{
    _color = color;
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

    vertices.push_back({ glm::vec2(0.0f, 0.0f)});

    for (int i = 0; i < _segmentCount; ++i)
    {
        float angle = 2.0f * glm::pi<float>() * i / _segmentCount;
        float x = _radius * cos(angle);
        float y = _radius * sin(angle);
        vertices.push_back({ glm::vec2(x, y)});
    }

    _vertices = vertices;
}

void Circle::CreateIndices()
{
    std::vector<uint16_t> indices;

    for (int i = 1; i < _segmentCount; ++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    indices.push_back(0);
    indices.push_back(_segmentCount);
    indices.push_back(1);

    _indices = indices;
}