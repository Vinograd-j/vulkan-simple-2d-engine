#pragma once

#include "logical-device.h"
#include "vulkan/vulkan.h"

class CommandPool
{

private:

    VkCommandPool _pool {};

    const LogicalDevice* const _device;

public:

    explicit CommandPool(const LogicalDevice* device, const VkCommandPoolCreateFlags& flags);

    VkCommandPool GetCommandPool() const { return _pool; }

    ~CommandPool();

private:

    void CreateCommandPool(const VkCommandPoolCreateFlags& flags);

};
