#pragma once

#include "buffer.h"
#include "../../../../engine/struct/storage-buffer.h"

class StorageBuffer
{

private:

    Buffer<StorageBufferObject> _buffer;

public:

    explicit StorageBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device);

    void AddBuffer(const StorageBufferObject& data, VkDeviceSize offset);

    VkBuffer GetBuffer() const { return _buffer.GetBuffer(); }

    void Update(const std::vector<StorageBufferObject>& data);

private:

    void CreateBuffer();

};
