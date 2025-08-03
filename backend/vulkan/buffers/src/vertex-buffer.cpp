#include "../include/vertex-buffer.h"

VertexBuffer::VertexBuffer(const Allocator* allocator, const std::vector<Vertex>& vertices, const CommandPool* pool, const LogicalDevice* device) :
                                                                                                                                                    _vertexBuffer(allocator, pool, device),
                                                                                                                                                    _vertices(vertices)
{
    CreateBuffer();
}

void VertexBuffer::CreateBuffer()
{
    _vertexBuffer.CreateBufferWithStaging(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, _vertices);
}