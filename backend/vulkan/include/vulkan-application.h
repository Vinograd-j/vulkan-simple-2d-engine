#pragma once

#include <string>
#include "vulkan/vulkan_core.h"

class VulkanApplication
{

private:

    std::string _appName;

    uint32_t _version;

    std::string _engineName;

    uint32_t _engineVersion;

    uint32_t _apiVersion;

private:

    VkApplicationInfo _appInfo {};

private:

    void CreateApplicationInfo(const void* pNext);

public:

    VulkanApplication(std::string appName, uint32_t version, std::string engineName, uint32_t engineVersion, uint32_t apiVersion, const void* pNext);

    VkApplicationInfo* GetApplicationInfo() { return &_appInfo; }

    uint32_t ApiVersion() const { return _apiVersion; }

};