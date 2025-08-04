#define VMA_IMPLEMENTATION

#include "../include/allocator.h"

#include <stdexcept>

Allocator::Allocator(const PhysicalDevice* physicalDevice, const LogicalDevice* device, const Instance* instance) : _physicalDevice(physicalDevice),
                                                                                                                    _device(device),
                                                                                                                    _instance(instance)
{
    CreateAllocator();
}

void Allocator::CreateBuffer(const VkBufferCreateInfo& createInfo, const VmaAllocationCreateInfo& allocationInfo, VkBuffer* buffer, VmaAllocation* memory, VmaAllocationInfo* allocInfo) const
{
    vmaCreateBuffer(_allocator, &createInfo, &allocationInfo, buffer, memory, allocInfo);
}

void Allocator::CreateImage(const VkImageCreateInfo& createInfo, const VmaAllocationCreateInfo& allocationInfo, VkImage* image, VmaAllocation* memory) const
{
    vmaCreateImage(_allocator, &createInfo, &allocationInfo, image, memory, nullptr);
}

void Allocator::CreateAllocator()
{
    VmaVulkanFunctions vulkanFunctions {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo createInfo {};
    createInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    createInfo.physicalDevice = _physicalDevice->GetPhysicalDevice();
    createInfo.device = _device->GetDevice();
    createInfo.instance = _instance->GetInstance();
    createInfo.pVulkanFunctions = &vulkanFunctions;

    if (vmaCreateAllocator(&createInfo, &_allocator) != VK_SUCCESS)
        throw std::runtime_error("failed to create vulkan memory allocator (vma)");
}

Allocator::~Allocator()
{
    vmaDestroyAllocator(_allocator);
}