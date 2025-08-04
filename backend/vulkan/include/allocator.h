#pragma once

#include "instance.h"
#include "logical-device.h"
#include "vk_mem_alloc.h"

class Allocator
{

private:

    VmaAllocator _allocator {};

private:

    const PhysicalDevice* _physicalDevice;
    const LogicalDevice* _device;

    const Instance* _instance;

public:

    explicit Allocator(const PhysicalDevice* physicalDevice, const LogicalDevice* device, const Instance* instance);

    VmaAllocator GetAllocator() const { return _allocator; }

    ~Allocator();

public:

    void CreateBuffer(const VkBufferCreateInfo& createInfo, const VmaAllocationCreateInfo& allocationInfo, VkBuffer* buffer, VmaAllocation* memory, VmaAllocationInfo
                      * allocInfo) const;

    void CreateImage(const VkImageCreateInfo& createInfo, const VmaAllocationCreateInfo& allocationInfo, VkImage* image, VmaAllocation*
                     memory) const;

private:

    void CreateAllocator();

};