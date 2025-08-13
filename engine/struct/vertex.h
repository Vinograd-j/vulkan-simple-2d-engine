#pragma once

#include <vulkan/vulkan_core.h>

#include "glm/glm.hpp"

struct Vertex
{
    glm::vec2 _pos;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 1> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, _pos);

        return attributeDescriptions;
    }

};