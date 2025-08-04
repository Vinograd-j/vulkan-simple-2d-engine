#pragma once

#include "vulkan/vulkan.hpp"

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR _capabilities;
    std::vector<VkSurfaceFormatKHR> _formats;
    std::vector<VkPresentModeKHR> _presentModes;
};
