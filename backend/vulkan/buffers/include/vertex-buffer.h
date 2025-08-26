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

    explicit VertexBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device);

    void AddVertices(const std::vector<Vertex>& vertices, VkDeviceSize offset);

    std::vector<Vertex> GetVertices() const { return _vertices; }

    VkBuffer GetBuffer() const { return _vertexBuffer.GetBuffer(); }

private:

    void CreateBuffer();

};
