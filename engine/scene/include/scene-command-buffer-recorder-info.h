#pragma once

#include "../../../backend/vulkan/buffers/include/index-buffer.h"
#include "../../../backend/vulkan/buffers/include/vertex-buffer.h"
#include "../../../backend/vulkan/command-buffer/include/command-buffers.h"
#include "../../pipeline/include/graphics-pipeline.h"
#include "../../struct/object-data.h"
#include "../../swapchain/include/present-swapchain.h"

struct SceneCommandBufferRecorderInfo
{
    const PresentSwapchain* _swapchain;

    const GraphicsPipeline* _pipeline;

    std::vector<ObjectData> _objects;

    CommandBuffers* buffers;

    std::vector<VkImageLayout>* _swapchainImageLayouts;

    std::vector<VkDescriptorSet> _descriptorSets;
};