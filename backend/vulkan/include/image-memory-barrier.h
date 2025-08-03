#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

class ImageMemoryBarrier
{

private:

    std::vector<VkImageLayout> _imageLayouts;

public:

    explicit ImageMemoryBarrier() {}

public:

    VkImageMemoryBarrier CreateBarrier(const VkImage& image, const VkImageLayout& oldLayout, const VkImageLayout& newLayout) const;

};
