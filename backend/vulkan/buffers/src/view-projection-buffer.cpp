#include "../include/view-projection-buffer.h"


UniformBuffer::UniformBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device, const ViewProjectionBuffer& initial) :
                                                                                                                                _buffer(allocator, pool, device)
{
    CreateBuffer(initial);
}

void UniformBuffer::CreateBuffer(const ViewProjectionBuffer& initial)
{
    _buffer.CreateBufferMapped(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, initial);
}

void UniformBuffer::Update(const ViewProjectionBuffer& data)
{
    _buffer.UpdateData(&data);
}
