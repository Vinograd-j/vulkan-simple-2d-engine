#pragma once

#include "buffer.h"
#include "../../../../engine/struct/uniform-object.h"

class UniformBuffer
{

private:

    Buffer<UniformObject> _buffer;

public:

    explicit UniformBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device, const UniformObject& initial);

    VkBuffer GetBuffer() const { return _buffer.GetBuffer(); }

    void Update(const UniformObject& data);

private:

    void CreateBuffer(const UniformObject& initial);

};
