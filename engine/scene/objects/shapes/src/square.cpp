#include "../include/square.h"

Square::Square()
{
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