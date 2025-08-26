#include "../include/index-buffer.h"

IndexBuffer::IndexBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device) :
                                                                                                            _indexBuffer(allocator, pool, device)
{
    CreateBuffer();
}

void IndexBuffer::AddIndices(const std::vector<uint16_t>& indices, VkDeviceSize offset)
{
    _indexBuffer.UploadData(indices, offset);
}

void IndexBuffer::CreateBuffer()
{
    VkDeviceSize size = 40ull * 1024ull * 1024ull;

    _indexBuffer.CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, size);
}