#pragma once
#include "allocator.h"
#include "buffer.h"
#include "../../../../engine/struct/vertex.h"


class VertexBuffer
{

private:

    Buffer<Vertex> _vertexBuffer;

    std::vector<Vertex> _vertices;

public:

    explicit VertexBuffer(const Allocator* allocator, const std::vector<Vertex>& vertices, const CommandPool* pool, const LogicalDevice* device);

    std::vector<Vertex> GetVertices() const { return _vertices; }

    VkBuffer GetBuffer() const { return _vertexBuffer.GetBuffer(); }

private:

    void CreateBuffer();

};
