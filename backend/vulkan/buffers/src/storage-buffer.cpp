#include "../include/storage-buffer.h"

StorageBuffer::StorageBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device, const std::vector<StorageBufferObject>& initial) :
                                                        _buffer(allocator, pool, device)
{
    CreateBuffer(initial);
}

void StorageBuffer::CreateBuffer(const std::vector<StorageBufferObject>& data)
{
    _buffer.CreateBufferMapped(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, data);
}

void StorageBuffer::Update(const std::vector<StorageBufferObject> &data)
{
    _buffer.UpdateData(data.data(), data.size());
}
