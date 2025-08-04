#include "../include/physical-device.h"

#include <set>

void PhysicalDevice::ChoosePhysicalDevice(uint32_t deviceID, uint32_t vendorID)
{
    if (_instance == nullptr)
        throw std::runtime_error("Given instance pointer is null!");

    uint32_t deviceCount = 0;

    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        throw std::runtime_error("There is no any physical device on this computer");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    bool found = false;
    for (const auto& device: devices)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.deviceID == deviceID && properties.vendorID == vendorID)
        {
            if (!IsDeviceSuitable(device))
                throw std::invalid_argument("provided device is not suitable for this program");

            _device = device;
            found = true;
            break;
        }
    }

    if (!found)
        throw std::runtime_error("No physical device with the specified deviceID found");
}

SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupportDetails(const VkPhysicalDevice& device) const
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details._capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details._formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details._formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details._presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details._presentModes.data());
    }

    return details;
}

std::map<FamilyType, uint32_t> PhysicalDevice::GetFamilies(const VkPhysicalDevice& device) const
{
    QueueFamilyIndices indices = FindQueueFamilies(device);

    std::map<FamilyType, uint32_t> familyTypeToIndex;

    if (indices._graphicsFamily)
        familyTypeToIndex[GRAPHICS] = indices._graphicsFamily.value();
    if (indices._computeFamily)
        familyTypeToIndex[COMPUTE] = indices._computeFamily.value();
    if (indices._presentFamily)
        familyTypeToIndex[PRESENT] = indices._presentFamily.value();
    if (indices._sparceBindingFamily)
        familyTypeToIndex[SPARCE_BINDING] = indices._sparceBindingFamily.value();
    if (indices._transferFamily)
        familyTypeToIndex[TRANSFER] = indices._transferFamily.value();

    return familyTypeToIndex;
}


QueueFamilyIndices PhysicalDevice::FindQueueFamilies(const VkPhysicalDevice& device) const
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    VkBool32 presentSupport = false;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices._graphicsFamily = i;

        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
            indices._computeFamily = i;

        if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
            indices._transferFamily = i;

        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
        if (presentSupport)
            indices._presentFamily = i;

        i++;
    }

    return indices;
}

bool PhysicalDevice::IsDeviceSuitable(const VkPhysicalDevice& device) const
{
    QueueFamilyIndices indices = FindQueueFamilies(device);

    bool extensionSupported = CheckDeviceExtensionsSupport(device);
    bool swapChainAdequate = false;
    if (extensionSupported)
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupportDetails(device);
        swapChainAdequate = !swapChainSupport._formats.empty() && !swapChainSupport._presentModes.empty();
    }

    return indices.IsComplete() && extensionSupported && swapChainAdequate;
}

bool PhysicalDevice::CheckDeviceExtensionsSupport(const VkPhysicalDevice& device) const
{
    uint32_t extensionsCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);

    std::vector< VkExtensionProperties> availableExtensions(extensionsCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(_extensions.cbegin(), _extensions.cend());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

VkPhysicalDeviceFeatures PhysicalDevice::GetFeatures(const VkPhysicalDevice device) const
{
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    return features;
}