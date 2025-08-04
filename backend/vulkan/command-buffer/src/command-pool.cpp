#include "../include/command-pool.h"

CommandPool::CommandPool(const LogicalDevice* device, const VkCommandPoolCreateFlags& flags) : _device(device)
{
    CreateCommandPool(flags);
}

void CommandPool::CreateCommandPool(const VkCommandPoolCreateFlags& flags)
{
    auto indices = _device->GetPhysicalDeviceIndices();

    VkCommandPoolCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.queueFamilyIndex = indices.at(GRAPHICS);

    if (vkCreateCommandPool(_device->GetDevice(), &createInfo, nullptr, &_pool) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool");
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(_device->GetDevice(), _pool, nullptr);
}