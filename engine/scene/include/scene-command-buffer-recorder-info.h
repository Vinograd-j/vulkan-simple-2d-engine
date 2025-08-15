#pragma once

#include "scene.h"
#include "../../../backend/vulkan/command-buffer/include/command-buffers.h"
#include "../../pipeline/include/graphics-pipeline.h"
#include "../../swapchain/include/present-swapchain.h"

struct SceneCommandBufferRecorderInfo
{
    const PresentSwapchain* _swapchain;

    const GraphicsPipeline* _pipeline;

    std::vector<ObjectData> _objects;

    VkBuffer _vertexBuffer;
    VkBuffer _indexBuffer;

    CommandBuffers* _buffers;

    std::vector<VkImageLayout>* _swapchainImageLayouts;

    std::vector<VkDescriptorSet> _descriptorSets;
};