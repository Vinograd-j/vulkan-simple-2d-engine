#include "../include/square.h"

Square::Square(const glm::vec3& color)
{
    _color = color;
    Square::CreateVertices();
    Square::CreateIndices();
}

void Square::CreateVertices()
{
    const std::vector<Vertex> vertices = {
        {{-0.2f, -0.2f} },
        {{0.2f, -0.2f} },
        {{0.2f, 0.2f}},
        {{-0.2f, 0.2f}}
    };

    _vertices = vertices;
}

void Square::CreateIndices()
{
    _indices = { 0, 1, 2, 2, 3, 0 };
}