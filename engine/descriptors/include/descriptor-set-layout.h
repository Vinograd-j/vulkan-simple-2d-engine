#pragma once

#include <vector>

#include "logical-device.h"
#include "vulkan/vulkan.h"

class DescriptorSetLayout
{

private:

    VkDescriptorSetLayout _layout {};

    std::vector<VkDescriptorSetLayoutBinding> _bindings {};

    const LogicalDevice* const _device;

public:

    explicit DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, const LogicalDevice* device);

    VkDescriptorSetLayout GetDescriptorLayout() const { return _layout; }

    ~DescriptorSetLayout();

private:

    void CreateDescriptorLayout();

};