#include "../include/uniform-buffer.h"


UniformBuffer::UniformBuffer(const Allocator* allocator, const CommandPool* pool, const LogicalDevice* device, const UniformObject& initial) :
                                                                                                                                _buffer(allocator, pool, device)
{
    CreateBuffer(initial);
}

void UniformBuffer::CreateBuffer(const UniformObject& initial)
{
    _buffer.CreateBufferMapped(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, initial);
}

void UniformBuffer::Update(const UniformObject& data)
{
    _buffer.UpdateData(&data);
}
