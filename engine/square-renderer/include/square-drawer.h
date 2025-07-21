#pragma once

#include "allocator.h"
#include "square-command-buffer-recorder.h"
#include "synchronization.h"
#include "../../pipeline/include/graphics-pipeline.h"
#include "../../swapchain/include/present-swapchain.h"
#include "../../renderer/command-buffer/include/command-buffers.h"
#include "../../renderer/command-buffer/include/command-pool.h"
#include "../../renderer/buffers/include/vertex-buffer.h"
#include "../../renderer/buffers/include/index-buffer.h"
#include "../../renderer/include/renderer.h"

class SquareDrawer : Renderer
{

private:

    const Allocator* _allocator;

    const CommandPool* const _pool;

    CommandBuffers _commandBuffers;

private:

    Synchronization _syncObjects;

    std::vector<VkImageView> _imageViews;

    std::unique_ptr<VertexBuffer> _vertexBuffer;
    std::unique_ptr<IndexBuffer> _indexBuffer;

    std::vector<VkImageLayout> _swapchainImageLayouts;

    std::unique_ptr<SquareCommandBufferRecorder> _recorder;

private:

    uint32_t _currentFrame = 0;

public:

    explicit SquareDrawer(const Allocator* allocator, const CommandPool* pool, const CommandBuffers& buffers, const GraphicsPipeline* pipeline, PresentSwapchain* swapchain, const LogicalDevice* device);

    void DrawFrame() override;

    ~SquareDrawer() override;

private:

    void CreateVertexBuffer();
    void CreateIndexBuffer();

    VkImageSubresourceRange GetImageSubresourceRange() const;

    VkComponentMapping GetComponentMapping() const;

    void RecreateSwapchain();

    void CreateBufferRecorder();

};