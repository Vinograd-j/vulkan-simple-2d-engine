#include "../include/storage-buffer.h"

StorageBuffer::StorageBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device) :
                                                                                                                _buffer(allocator, pool, device)
{
    CreateBuffer();
}

void StorageBuffer::CreateBuffer()
{
    VkDeviceSize size = 40ull * 1024ull * 1024ull;

    _buffer.CreateBufferMapped(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, size);
}

void StorageBuffer::AddBuffer(const StorageBufferObject& data, VkDeviceSize offset)
{
    _buffer.UploadData(&data, offset);
}

void StorageBuffer::Update(const std::vector<StorageBufferObject> &data)
{
    _buffer.UpdateData(data.data(), data.size());
}