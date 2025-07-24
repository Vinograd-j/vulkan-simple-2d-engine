#include "../include/circle-drawer.h"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>

#include "../../struct/uniform-object.h"

CircleDrawer::CircleDrawer(const Allocator* allocator, const CommandPool* pool, const CommandBuffers& buffers, const GraphicsPipeline* pipeline, PresentSwapchain* swapchain, const LogicalDevice* device, const VkDescriptorSetLayout& layout) :
                                                                                                                            Renderer(pipeline, swapchain, device),
                                                                                                                            _allocator(allocator),
                                                                                                                            _commandPool(pool),
                                                                                                                            _commandBuffers(buffers),
                                                                                                                            _syncObjects(FRAMES_IN_FLIGHT, device),
                                                                                                                            _descriptorSetLayout(layout)
{
    _swapchainImageLayouts.resize(_swapchain->GetSwapchainImages().size(), VK_IMAGE_LAYOUT_UNDEFINED);

    CreateVertexBuffer(0.3f, 64);
    CreateIndexBuffer(64);
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateBufferRecorder();

    auto subresourceRange = GetImageSubresourceRange();
    auto mapping = GetComponentMapping();
    _imageViews = _swapchain->GetImageViews(subresourceRange, mapping);
}

void CircleDrawer::DrawFrame()
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

    UpdateUniformBuffer(_currentFrame);

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

    _currentFrame = (_currentFrame + 1) % FRAMES_IN_FLIGHT;
}

void CircleDrawer::CreateVertexBuffer(float radius, int segmentCount)
{
    std::vector<Vertex> vertices;

    vertices.push_back({ glm::vec2(0.0f, 0.0f), {1.0f, 0.0f, 0.0f} });

    for (int i = 0; i <= segmentCount; ++i)
    {
        float angle = 2.0f * glm::pi<float>() * i / segmentCount;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        vertices.push_back({ glm::vec2(x, y),  {0.0f, 1.0f, 0.0f} });
    }


    _vertexBuffer = std::make_unique<VertexBuffer>(_allocator, vertices, _commandPool, _device);
}

void CircleDrawer::CreateIndexBuffer(int segmentCount)
{
    std::vector<uint16_t> indices;

    for (int i = 1; i <= segmentCount; ++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    _indexBuffer = std::make_unique<IndexBuffer>(_allocator, indices, _commandPool, _device);
}

VkImageSubresourceRange CircleDrawer::GetImageSubresourceRange() const
{
    VkImageSubresourceRange subresourceRange {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    return subresourceRange;
}

VkComponentMapping CircleDrawer::GetComponentMapping() const
{
    VkComponentMapping mapping {};
    mapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    mapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    mapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    mapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    return mapping;
}

void CircleDrawer::RecreateSwapchain()
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

void CircleDrawer::CreateBufferRecorder()
{
    CircleCommandBufferRecorderInfo squareCommandBufferRecorderInfo;
    squareCommandBufferRecorderInfo.pool = _commandPool;
    squareCommandBufferRecorderInfo.buffers = &_commandBuffers;
    squareCommandBufferRecorderInfo._pipeline = _pipeline;
    squareCommandBufferRecorderInfo._swapchain = _swapchain;
    squareCommandBufferRecorderInfo._indexBuffer = _indexBuffer.get();
    squareCommandBufferRecorderInfo._vertexBuffer = _vertexBuffer.get();
    squareCommandBufferRecorderInfo._swapchainImageLayouts = &_swapchainImageLayouts;
    squareCommandBufferRecorderInfo._descriptorSets = _descriptorSets;

    _recorder = std::make_unique<CircleCommandBufferRecorder>(squareCommandBufferRecorderInfo);
}

void CircleDrawer::CreateUniformBuffers()
{
    _uniformBuffers.resize(FRAMES_IN_FLIGHT);

    UniformObject ubo {};
    for (int i = 0; i < _uniformBuffers.size(); ++i)
        _uniformBuffers[i] = std::make_unique<UniformBuffer>(_allocator, _commandPool, _device, ubo);
}

void CircleDrawer::CreateDescriptorPool()
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

void CircleDrawer::UpdateUniformBuffer(uint32_t currentFrame) const
{
    UniformObject ubo{};

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();

    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    float amplitude = 0.4f;
    float speed = 0.9;
    float xOffset = std::sin(time * speed) * amplitude;

    float angle = cos(time * speed) * glm::pi<float>() / 12;

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 translation2 = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset, 0.0f, 0.0f));

    glm::mat4 model = rotation * translation2;

    float aspect = _swapchain->GetExtent().width / static_cast<float>(_swapchain->GetExtent().height);
    glm::mat4 aspectFix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / aspect, 1.0f, 1.0f));
    model = aspectFix * rotation * translation2;
    ubo._model = model;

    ubo._model = model;

    _uniformBuffers[currentFrame]->Update(ubo);
}

void CircleDrawer::CreateDescriptorSets()
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
        bufferInfo.buffer = _uniformBuffers[i].get()->GetBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformObject);

        VkWriteDescriptorSet descriptorWrite {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = _descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(_device->GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

CircleDrawer::~CircleDrawer()
{
    for (auto imageView : _imageViews)
        vkDestroyImageView(_device->GetDevice(), imageView, nullptr);

    for (auto& buffer : _uniformBuffers)
        buffer.reset();

    _vertexBuffer.reset();
    _indexBuffer.reset();
    vkDestroyDescriptorPool(_device->GetDevice(), _descriptorPool, nullptr);

    _recorder.reset();
}