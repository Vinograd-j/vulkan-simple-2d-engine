#pragma once

#include "logical-device.h"
#include "swap-chain-support-details.h"
#include "window.h"

class ISwapchain
{

protected:

    VkSwapchainKHR _swapchain {};

    VkPresentModeKHR _presentMode {};
    VkSurfaceFormatKHR _surfaceFormat {};
    VkExtent2D _extent {};

    std::vector<VkImage> _swapchainImages;

    SwapChainSupportDetails _swapChainSupportDetails {};

protected:

    const PhysicalDevice* const _physicalDevice;
    const LogicalDevice* const _device;
    VkSurfaceKHR _surface;

    GLFWwindow* const _window;

public:

    explicit ISwapchain(const PhysicalDevice* physicalDevice, const LogicalDevice* device, GLFWwindow* window, const VkSurfaceKHR& surface) :
                                                                                                                                             _physicalDevice(physicalDevice),
                                                                                                                                             _device(device),
                                                                                                                                             _surface(surface),
                                                                                                                                             _window(window) {}

    virtual ~ISwapchain() = default;

public:

    virtual size_t GetImageCount() const = 0;
    virtual VkExtent2D GetExtent() const = 0;
    virtual VkFormat GetImageFormat() const = 0;
    virtual std::vector<VkImage>  GetSwapchainImages() const = 0;
    virtual void CleanupSwapchain() = 0;

    virtual std::vector<VkImageView> GetImageViews(const VkImageSubresourceRange& subresourceRange, const VkComponentMapping& components) const = 0;

private:

    virtual void CreateSwapchain(const VkSwapchainKHR& oldSwapchain) = 0;

};
