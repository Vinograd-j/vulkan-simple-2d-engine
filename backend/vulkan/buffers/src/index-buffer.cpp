#include "../include/index-buffer.h"

IndexBuffer::IndexBuffer(const Allocator* allocator, const std::vector<uint16_t>& vertices, const CommandPool* pool, const LogicalDevice* device) :
                                                                                                                                                    _indices(vertices),
                                                                                                                                                    _indexBuffer(allocator, pool, device)
{
    CreateBuffer();
}

void IndexBuffer::CreateBuffer()
{
    _indexBuffer.CreateBufferWithStaging(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, _indices);
}