#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "im-gui-command-buffer-recorder.h"
#include "../../swapchain/include/present-swapchain.h"
#include "../../vulkan/command-buffer/include/command-buffers.h"
#include "../../vulkan/include/instance.h"
#include "../../vulkan/include/logical-device.h"
#include "../../vulkan/include/window.h"

class ImGUI
{

private:

    const PresentSwapchain* const _swapchain;

    const Window* const _window;

    const PhysicalDevice* const _physicalDevice;
    const LogicalDevice* const  _device;

    const Instance* const _instance;

    std::unique_ptr<CommandBuffers> _commandBuffers;

    VkDescriptorPool _descriptorPool;

    VkCommandPool _commandPool;

    std::unique_ptr<ImGUICommandBufferRecorder> _recorder;

public:

    explicit ImGUI(const PresentSwapchain* swapchain, const Window* window, const PhysicalDevice* physicalDevice, const LogicalDevice* logicalDevice,
        const Instance* instance, const VkCommandPool& commandPool);

    ~ImGUI();

    VkCommandBuffer PrepareCommandBuffer(uint32_t imageIndex, const VkImageView& imageView) const;

private:

    void InitImGUI();

    void CreateDescriptorPool();

    void CreateCommandBufferRecorder();

    void Destroy();

};