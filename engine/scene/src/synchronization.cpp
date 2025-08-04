#include "../include/synchronization.h"

Synchronization::Synchronization(uint32_t maxFramesInFlight, const LogicalDevice* device) : _device(device)
{
    CreateSyncObjects(maxFramesInFlight);
}

void Synchronization::CreateSyncObjects(uint32_t maxFramesInFlight)
{
    _imageAvailableSemaphores.resize(maxFramesInFlight);
    _renderingFinishedSemaphores.resize(maxFramesInFlight);
    _inFlightFences.resize(maxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreCreateInfo {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < maxFramesInFlight; ++i)
    {
        if (vkCreateSemaphore(_device->GetDevice(), &semaphoreCreateInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_device->GetDevice(), &semaphoreCreateInfo, nullptr, &_renderingFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(_device->GetDevice(), &fenceCreateInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphore or fence for a frame");
        }
    }
}

void Synchronization::WaitForFence(uint32_t currentFrame) const
{
    vkWaitForFences(_device->GetDevice(), 1, &_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
}

void Synchronization::ResetFence(uint32_t index) const
{
    vkResetFences(_device->GetDevice(), 1, &_inFlightFences[index]);
}

Synchronization::~Synchronization()
{
    for (auto semaphore : _imageAvailableSemaphores)
        vkDestroySemaphore(_device->GetDevice(), semaphore, nullptr);

    for (auto semaphore : _renderingFinishedSemaphores)
        vkDestroySemaphore(_device->GetDevice(), semaphore, nullptr);

    for (auto fence : _inFlightFences)
        vkDestroyFence(_device->GetDevice(), fence, nullptr);
}
