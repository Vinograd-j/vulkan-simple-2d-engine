#include "../include/descriptor-set-layout.h"

#include <stdexcept>

DescriptorSetLayout::DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, const LogicalDevice* device) : _bindings(bindings), _device(device)
{
    CreateDescriptorLayout();
}

void DescriptorSetLayout::CreateDescriptorLayout()
{
    VkDescriptorSetLayoutCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = _bindings.size();
    createInfo.pBindings = _bindings.data();

    if (vkCreateDescriptorSetLayout(_device->GetDevice(), &createInfo, nullptr, &_layout) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor set layout");
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(_device->GetDevice(), _layout, nullptr);
}