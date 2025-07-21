#pragma once

#include "../../pipeline/include/graphics-pipeline.h"
#include "../../renderer/buffers/include/index-buffer.h"
#include "../../renderer/buffers/include/vertex-buffer.h"
#include "../../renderer/command-buffer/include/command-buffers.h"
#include "../../swapchain/include/present-swapchain.h"

struct SquareCommandBufferRecorderInfo
{
    const PresentSwapchain* _swapchain;

    const GraphicsPipeline* _pipeline;

    const VertexBuffer* _vertexBuffer;

    const IndexBuffer* _indexBuffer;

    const CommandPool* pool;

    CommandBuffers* buffers;

    std::vector<VkImageLayout>* _swapchainImageLayouts;
};