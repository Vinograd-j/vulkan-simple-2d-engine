#pragma once

#include <vector>

#include "buffer.h"
#include "../../../../backend/vulkan/include/allocator.h"

class IndexBuffer
{

private:

    Buffer<uint16_t> _indexBuffer;

    std::vector<uint16_t> _indices;

public:

    explicit IndexBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device);

    void AddIndices(const std::vector<uint16_t>& indices, VkDeviceSize offset);

    std::vector<uint16_t> GetIndices() const { return _indices; }

    VkBuffer GetBuffer() const { return _indexBuffer.GetBuffer(); }

private:

    void CreateBuffer();

};
