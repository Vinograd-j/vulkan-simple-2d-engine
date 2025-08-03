#include "../include/command-buffers.h"

#include <stdexcept>


CommandBuffers::CommandBuffers(uint32_t count, const VkCommandBufferLevel& level, VkCommandPool pool, const LogicalDevice* device) : _pool(pool), _device(device)
{
    CreateCommandBuffers(level, count);
}

void CommandBuffers::CreateCommandBuffers(const VkCommandBufferLevel& level, uint32_t count)
{
    VkCommandBufferAllocateInfo allocateInfo {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = _pool;
    allocateInfo.level = level;
    allocateInfo.commandBufferCount = count;

    _commandBuffers.resize(count);

    if (vkAllocateCommandBuffers(_device->GetDevice(), &allocateInfo, _commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to create command buffers");
}