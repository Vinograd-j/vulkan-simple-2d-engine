#include "../include/image-memory-barrier.h"

VkImageMemoryBarrier ImageMemoryBarrier::CreateBarrier(const VkImage& image, const VkImageLayout& oldLayout, const VkImageLayout& newLayout) const
{
    VkImageMemoryBarrier barrierToRender{};
    barrierToRender.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierToRender.oldLayout = oldLayout;
    barrierToRender.newLayout = newLayout;
    barrierToRender.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierToRender.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierToRender.image = image;
    barrierToRender.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierToRender.subresourceRange.baseMipLevel = 0;
    barrierToRender.subresourceRange.levelCount = 1;
    barrierToRender.subresourceRange.baseArrayLayer = 0;
    barrierToRender.subresourceRange.layerCount = 1;

    return barrierToRender;
}