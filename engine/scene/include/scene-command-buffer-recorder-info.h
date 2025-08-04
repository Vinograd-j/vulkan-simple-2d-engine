#pragma once

#include "../../../backend/vulkan/buffers/include/index-buffer.h"
#include "../../../backend/vulkan/buffers/include/vertex-buffer.h"
#include "../../../backend/vulkan/command-buffer/include/command-buffers.h"
#include "../../pipeline/include/graphics-pipeline.h"
#include "../../swapchain/include/present-swapchain.h"

struct SceneCommandBufferRecorderInfo
{
    const PresentSwapchain* _swapchain;

    const GraphicsPipeline* _pipeline;

    const VertexBuffer* _vertexBuffer;

    const IndexBuffer* _indexBuffer;

    CommandBuffers* buffers;

    std::vector<VkImageLayout>* _swapchainImageLayouts;

    std::vector<VkDescriptorSet> _descriptorSets;
};