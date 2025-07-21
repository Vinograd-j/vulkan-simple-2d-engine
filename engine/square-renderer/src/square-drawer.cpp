#include "../include/square-drawer.h"

SquareDrawer::SquareDrawer(const Allocator* allocator, const CommandPool* pool, const CommandBuffers& buffers, const GraphicsPipeline* pipeline, PresentSwapchain* swapchain, const LogicalDevice* device) :
                                                                                                                            Renderer(pipeline, swapchain, device),
                                                                                                                            _allocator(allocator),
                                                                                                                            _pool(pool),
                                                                                                                            _commandBuffers(buffers),
                                                                                                                            _syncObjects(2, device)
{
    _swapchainImageLayouts.resize(_swapchain->GetSwapchainImages().size(), VK_IMAGE_LAYOUT_UNDEFINED);

    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateBufferRecorder();

    auto subresourceRange = GetImageSubresourceRange();
    auto mapping = GetComponentMapping();
    _imageViews = _swapchain->GetImageViews(subresourceRange, mapping);
}

void SquareDrawer::DrawFrame()
{
    const VkQueue graphicsQueue = _device->GetQueues().at(GRAPHICS);
    const VkQueue presentQueue = _device->GetQueues().at(PRESENT);

    _syncObjects.WaitForFence(_currentFrame);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(_device->GetDevice(), _swapchain->GetSwapchain(), UINT64_MAX, _syncObjects.ImageAvailableSemaphores()[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("failed to acquire swap chain image");

    _syncObjects.ResetFence(_currentFrame);

    _recorder->RecordCommandBuffer(_currentFrame, _imageViews[imageIndex], imageIndex);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_syncObjects.ImageAvailableSemaphores()[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffers[_currentFrame];

    VkSemaphore signalSemaphores[] = {_syncObjects.RenderingFinishedSemaphores()[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, _syncObjects.InFlightFences()[_currentFrame]) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer");

    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {_swapchain->GetSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        RecreateSwapchain();
        return;
    }

    if (result != VK_SUCCESS)
        throw std::runtime_error("failed to acquire swap chain image");

    _currentFrame = (_currentFrame + 1) % 2;
}

void SquareDrawer::CreateVertexBuffer()
{
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    _vertexBuffer = std::make_unique<VertexBuffer>(_allocator, vertices, _pool, _device);
}

void SquareDrawer::CreateIndexBuffer()
{
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    _indexBuffer = std::make_unique<IndexBuffer>(_allocator, indices, _pool, _device);
}

VkImageSubresourceRange SquareDrawer::GetImageSubresourceRange() const
{
    VkImageSubresourceRange subresourceRange {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    return subresourceRange;
}

VkComponentMapping SquareDrawer::GetComponentMapping() const
{
    VkComponentMapping mapping {};
    mapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    mapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    mapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    mapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    return mapping;
}

void SquareDrawer::RecreateSwapchain()
{
    _swapchain->Recreate();

    auto subresourceRange = GetImageSubresourceRange();
    auto mapping = GetComponentMapping();

    for (auto imageView : _imageViews)
        vkDestroyImageView(_device->GetDevice(), imageView, nullptr);

    _imageViews = _swapchain->GetImageViews(subresourceRange, mapping);

    _swapchainImageLayouts.clear();
    _swapchainImageLayouts.resize(_imageViews.size(), VK_IMAGE_LAYOUT_UNDEFINED);
}

void SquareDrawer::CreateBufferRecorder()
{
    SquareCommandBufferRecorderInfo squareCommandBufferRecorderInfo;
    squareCommandBufferRecorderInfo.pool = _pool;
    squareCommandBufferRecorderInfo.buffers = &_commandBuffers;
    squareCommandBufferRecorderInfo._pipeline = _pipeline;
    squareCommandBufferRecorderInfo._swapchain = _swapchain;
    squareCommandBufferRecorderInfo._indexBuffer = _indexBuffer.get();
    squareCommandBufferRecorderInfo._vertexBuffer = _vertexBuffer.get();
    squareCommandBufferRecorderInfo._swapchainImageLayouts = &_swapchainImageLayouts;

    _recorder = std::make_unique<SquareCommandBufferRecorder>(squareCommandBufferRecorderInfo);
}

SquareDrawer::~SquareDrawer()
{
    for (auto imageView : _imageViews)
        vkDestroyImageView(_device->GetDevice(), imageView, nullptr);

    _recorder.reset();
}