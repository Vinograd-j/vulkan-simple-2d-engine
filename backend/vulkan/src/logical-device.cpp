#include "../include/logical-device.h"

#include <set>
#include <unordered_map>

LogicalDevice::LogicalDevice(const PhysicalDevice& physicalDevice) : _physicalDevice(physicalDevice)
{
    CreateLogicalDevice();
}

void LogicalDevice::CreateLogicalDevice()
{
    auto families = _physicalDevice.GetFamilies(_physicalDevice.GetPhysicalDevice());
    std::vector<const char*> extensions = _physicalDevice.GetExtensions();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueFamilies;
    float queuePriority = 1.0f;

    for (const auto& [type, index] : families)
    {
        if (uniqueFamilies.insert(index).second)
        {
            VkDeviceQueueCreateInfo createInfo {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.queueFamilyIndex = index;
            createInfo.queueCount = 1;
            createInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(createInfo);
        }
    }

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures {};
    dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceFeatures supportedFeatures {};
    vkGetPhysicalDeviceFeatures(_physicalDevice.GetPhysicalDevice(), &supportedFeatures);
    createInfo.pEnabledFeatures = &supportedFeatures;
    createInfo.pNext = &dynamicRenderingFeatures;

    if (vkCreateDevice(_physicalDevice.GetPhysicalDevice(), &createInfo, nullptr, &_device) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device");

    std::unordered_map<uint32_t, VkQueue> queuesByFamily;

    for (uint32_t familyIndex : uniqueFamilies)
    {
        VkQueue queue;
        vkGetDeviceQueue(_device, familyIndex, 0, &queue);
        queuesByFamily[familyIndex] = queue;
    }

    for (const auto& [type, familyIndex] : families)
        _queues[type] = queuesByFamily[familyIndex];
}

LogicalDevice::~LogicalDevice()
{
    if (_device != VK_NULL_HANDLE)
        vkDestroyDevice(_device, nullptr);
}