#include "../include/vulkan-application.h"

VulkanApplication::VulkanApplication(std::string appName, uint32_t version, std::string engineName, uint32_t engineVersion, uint32_t apiVersion, const void* pNext) :
                                                                                                                              _appName(std::move(appName)),
                                                                                                                              _version(version),
                                                                                                                              _engineName(std::move(engineName)),
                                                                                                                              _engineVersion(engineVersion),
                                                                                                                              _apiVersion(apiVersion)
{
    CreateApplicationInfo(pNext);
}


void VulkanApplication::CreateApplicationInfo(const void* pNext)
{
    _appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    _appInfo.pApplicationName = _appName.c_str();
    _appInfo.applicationVersion = _apiVersion;
    _appInfo.pEngineName = _engineName.c_str();
    _appInfo.engineVersion = _engineVersion;
    _appInfo.apiVersion = _apiVersion;
    _appInfo.pNext = pNext;
}