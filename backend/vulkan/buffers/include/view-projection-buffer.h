#pragma once

#include "buffer.h"

#include "../../../../engine/struct/view-projection-buffer.h"

class UniformBuffer
{

private:

    Buffer<ViewProjectionBuffer> _buffer;

public:

    explicit UniformBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device, const ViewProjectionBuffer& initial);

    VkBuffer GetBuffer() const { return _buffer.GetBuffer(); }

    void Update(const ViewProjectionBuffer& data);

private:

    void CreateBuffer(const ViewProjectionBuffer& initial);

};
