#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "logical-device.h"

class Synchronization
{

private:

    std::vector<VkFence> _inFlightFences;

    std::vector<VkSemaphore> _renderingFinishedSemaphores;

    std::vector<VkSemaphore> _imageAvailableSemaphores;

private:

    const LogicalDevice* const _device;

public:

    explicit Synchronization(uint32_t maxFramesInFlight, const LogicalDevice* device);

    void WaitForFence(uint32_t currentFrame) const;

    void ResetFence(uint32_t index) const;


    const std::vector<VkFence>& InFlightFences() const { return _inFlightFences; }
    const std::vector<VkSemaphore>& RenderingFinishedSemaphores() const { return _renderingFinishedSemaphores; }
    const std::vector<VkSemaphore>& ImageAvailableSemaphores() const { return _imageAvailableSemaphores; }
    const LogicalDevice* Device() const { return _device; }

    ~Synchronization();

private:

    void CreateSyncObjects(uint32_t maxFramesInFlight);

};
