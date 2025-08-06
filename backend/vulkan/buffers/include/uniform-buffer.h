#pragma once

#include "buffer.h"
#include "../../../../engine/struct/object-data.h"

#include "../../../../engine/struct/UniformBuffer.h"

class UniformBuffer
{

private:

    Buffer<UniformObject> _buffer;

public:

    explicit UniformBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device, const ObjectData& initial);

    VkBuffer GetBuffer() const { return _buffer.GetBuffer(); }

    void Update(const UniformObject& data);

private:

    void CreateBuffer(const UniformObject& initial);

};
