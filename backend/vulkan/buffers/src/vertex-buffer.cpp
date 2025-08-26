#include "../include/vertex-buffer.h"

VertexBuffer::VertexBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device) :
                                                                                                               _vertexBuffer(allocator, pool, device)
{
    CreateBuffer();
}

void VertexBuffer::AddVertices(const std::vector<Vertex>& vertices, VkDeviceSize offset)
{
    _vertexBuffer.UploadData(vertices, offset);
}

void VertexBuffer::CreateBuffer()
{
    VkDeviceSize size = 40ull * 1024ull * 1024ull;

    _vertexBuffer.CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, size);
}