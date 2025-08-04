#pragma once

#include <map>

#include "queue-family-indices.h"
#include "Vulkan/vulkan.hpp"

#include  "swap-chain-support-details.h"

class PhysicalDevice
{

private:

    VkPhysicalDevice _device {};

    QueueFamilyIndices _familyIndices;

private:

    VkInstance _instance;

    VkSurfaceKHR _surface;

    std::vector<const char*> _extensions;

public:

    explicit PhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface, const std::vector<const char*>& requiredExtensions) :
                                                                                                                                          _instance(instance),
                                                                                                                                          _surface(surface),
                                                                                                                                          _extensions(requiredExtensions){}

    void ChoosePhysicalDevice(uint32_t deviceID, uint32_t vendorID);


    SwapChainSupportDetails QuerySwapChainSupportDetails(const VkPhysicalDevice& device) const;
    std::map<FamilyType, uint32_t> GetFamilies(const VkPhysicalDevice& device) const;

    VkPhysicalDevice GetPhysicalDevice() const { return _device; }
    std::vector<const char*> GetExtensions() const { return _extensions; }
    VkPhysicalDeviceFeatures GetFeatures(VkPhysicalDevice device) const;

    ~PhysicalDevice() = default;

private:

    QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device) const;

    bool IsDeviceSuitable(const VkPhysicalDevice& device) const;

    bool CheckDeviceExtensionsSupport(const VkPhysicalDevice& device) const;

};