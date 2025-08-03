#include "../include/instance.h"

void Instance::CreateInstance()
{
    VkInstanceCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = _application.GetApplicationInfo();
    createInfo.enabledExtensionCount = _extensions.size();
    createInfo.ppEnabledExtensionNames = _extensions.data();
    createInfo.enabledLayerCount = _layers.size();
    createInfo.ppEnabledLayerNames = _layers.data();
    createInfo.pNext = _pNext;
    createInfo.flags = _flags;

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Instance");
}

Instance::~Instance()
{
    vkDestroyInstance(_instance, nullptr);
}
