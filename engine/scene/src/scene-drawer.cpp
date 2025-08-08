#include "../include/scene-drawer.h"

#include <chrono>
#include <imgui_impl_vulkan.h>
#include <random>
#include "../../../backend/vulkan/buffers/include/storage-buffer.h"
#include "../../../backend/vulkan/buffers/include/uniform-buffer.h"
#include "../../../backend/vulkan/include/allocator.h"
#include "../../struct/object-data.h"
#include "../objects/shapes/include/circle.h"

SceneDrawer::SceneDrawer(const Allocator* allocator, const CommandPool* pool, const CommandBuffers& buffers, const GraphicsPipeline* pipeline, PresentSwapchain* swapchain, const LogicalDevice* device, const VkDescriptorSetLayout& layout, const Gui* gui) :
                                                                                                                            Renderer(pipeline, swapchain, device),
                                                                                                                            _allocator(allocator),
                                                                                                                            _commandPool(pool),
                                                                                                                            _commandBuffers(buffers),
                                                                                                                            _gui(gui),
                                                                                                                            _syncObjects(FRAMES_IN_FLIGHT, device),
                                                                                                                            _descriptorSetLayout(layout)
{
    _swapchainImageLayouts.resize(_swapchain->GetSwapchainImages().size(), VK_IMAGE_LAYOUT_UNDEFINED);

    CreateSSBO();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateBufferRecorder();

    auto subresourceRange = GetImageSubresourceRange();
    auto mapping = GetComponentMapping();
    _imageViews = _swapchain->GetImageViews(subresourceRange, mapping);
}

void SceneDrawer::DrawFrame()
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

    //UpdateUniformBuffer(_currentFrame);

    _syncObjects.ResetFence(_currentFrame);

    //_gui->DrawSceneGUI(std::bind(SceneDrawer::ChangeCircleColor, this));

    _recorder->RecordCommandBuffer(_currentFrame, _imageViews[imageIndex], imageIndex);

    VkCommandBuffer guiBuffer = _gui->PrepareCommandBuffer(imageIndex, _imageViews[imageIndex]);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkCommandBuffer submitBuffers[] = {_commandBuffers[_currentFrame], guiBuffer};

    VkSemaphore waitSemaphores[] = {_syncObjects.ImageAvailableSemaphores()[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 2;
    submitInfo.pCommandBuffers = submitBuffers;

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

    _currentFrame = (_currentFrame + 1) % FRAMES_IN_FLIGHT;
}

void SceneDrawer::CreateSSBO()
{
    _storageBuffer.resize(FRAMES_IN_FLIGHT);

    ObjectData ubo {};
    for (int i = 0; i < _storageBuffer.size(); ++i)
        _storageBuffer[i] = std::make_unique<StorageBuffer>(_allocator, _commandPool, _device, _scene->GetObjectsSSBO());
}

VkImageSubresourceRange SceneDrawer::GetImageSubresourceRange() const
{
    VkImageSubresourceRange subresourceRange {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    return subresourceRange;
}

VkComponentMapping SceneDrawer::GetComponentMapping() const
{
    VkComponentMapping mapping {};
    mapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    mapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    mapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    mapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    return mapping;
}

void SceneDrawer::RecreateSwapchain()
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

void SceneDrawer::CreateBufferRecorder()
{
    SceneCommandBufferRecorderInfo squareCommandBufferRecorderInfo;
    squareCommandBufferRecorderInfo._buffers = &_commandBuffers;
    squareCommandBufferRecorderInfo._pipeline = _pipeline;
    squareCommandBufferRecorderInfo._swapchain = _swapchain;
    squareCommandBufferRecorderInfo._objects = _scene->GetObjectsSSBO();
    squareCommandBufferRecorderInfo._vertexBuffer = _scene->GetObjectsSSBO();
    squareCommandBufferRecorderInfo._indexBuffer = _scene->GetObjectsSSBO();
    squareCommandBufferRecorderInfo._swapchainImageLayouts = &_swapchainImageLayouts;
    squareCommandBufferRecorderInfo._descriptorSets = _descriptorSets;

    _recorder = std::make_unique<SceneCommandBufferRecorder>(squareCommandBufferRecorderInfo);
}

void SceneDrawer::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    createInfo.maxSets = FRAMES_IN_FLIGHT;

    if (vkCreateDescriptorPool(_device->GetDevice(), &createInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool");
}

void SceneDrawer::CreateScene()
{
    std::vector<std::unique_ptr<Renderable>> objects;
    objects.push_back(std::make_unique<Circle>(5, 36));

    _scene = std::make_unique<Scene>(objects);
}

void SceneDrawer::CreateVertexBuffer()
{
    _vertexBuffer = std::make_unique<VertexBuffer>(_allocator, _scene->GetAllVertices(), _commandPool, _device);
}

void SceneDrawer::CreateIndexBuffer()
{
    _indexBuffer = std::make_unique<IndexBuffer>(_allocator, _scene->GetAllIndices(), _commandPool, _device);
}

void SceneDrawer::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(FRAMES_IN_FLIGHT, _descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    _descriptorSets.resize(FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(_device->GetDevice(), &allocInfo, _descriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets");

    for (int i = 0; i < FRAMES_IN_FLIGHT; ++i)
    {
        VkDescriptorBufferInfo bufferInfo {};
        bufferInfo.buffer = _storageBuffer[i].get()->GetBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(ObjectData) * _scene->GetObjectsSSBO().size();

        VkWriteDescriptorSet descriptorWrite {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = _descriptorSets[i];
        descriptorWrite.dstBinding = 1;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(_device->GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

SceneDrawer::~SceneDrawer()
{
    for (auto imageView : _imageViews)
        vkDestroyImageView(_device->GetDevice(), imageView, nullptr);

    for (auto& buffer : _storageBuffer)
        buffer.reset();

    _scene.reset();

    vkDestroyDescriptorPool(_device->GetDevice(), _descriptorPool, nullptr);

    _recorder.reset();
}