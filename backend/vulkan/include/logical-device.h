#pragma once

#include "physical-device.h"

class LogicalDevice
{

private:

    VkDevice _device {};

private:

    PhysicalDevice _physicalDevice;

    std::map<FamilyType, VkQueue> _queues;

public:

    explicit LogicalDevice(const PhysicalDevice& physicalDevice);

    ~LogicalDevice();

    VkDevice GetDevice() const { return _device; }
    
    std::map<FamilyType, VkQueue> GetQueues() const { return _queues; }

    std::map<FamilyType, uint32_t> GetPhysicalDeviceIndices() const { return _physicalDevice.GetFamilies(_physicalDevice.GetPhysicalDevice()); }

private:

    void CreateLogicalDevice();

};
