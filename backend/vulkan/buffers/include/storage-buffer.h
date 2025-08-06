#pragma once
#include "buffer.h"
#include "../../../../engine/struct/object-data.h"

class StorageBuffer
{

private:

    Buffer<ObjectData> _buffer;

public:

    explicit StorageBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device, const std::vector<ObjectData>& initial);

    VkBuffer GetBuffer() const { return _buffer.GetBuffer(); }

    void Update(const std::vector<ObjectData>& data);

private:

    void CreateBuffer(const std::vector<ObjectData>& data);

};
