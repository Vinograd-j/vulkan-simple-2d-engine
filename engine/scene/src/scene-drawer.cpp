#include "../include/scene-drawer.h"

#include <chrono>
#include <imgui_impl_vulkan.h>
#include <iostream>
#include <random>

#include "../../../backend/vulkan/buffers/include/storage-buffer.h"
#include "../../../backend/vulkan/buffers/include/uniform-buffer.h"
#include "../../../backend/vulkan/include/allocator.h"
#include "../../struct/storage-buffer.h"
#include "../objects/shapes/include/circle.h"

SceneDrawer::SceneDrawer(const Allocator* allocator, const CommandPool* pool, const CommandBuffers& buffers, const GraphicsPipeline* pipeline, PresentSwapchain* swapchain, const LogicalDevice* device, const VkDescriptorSetLayout& layout, Gui* gui) :
                                                                                                                            Renderer(pipeline, swapchain, device),
                                                                                                                            _allocator(allocator),
                                                                                                                            _commandPool(pool),
                                                                                                                            _commandBuffers(buffers),
                                                                                                                            _gui(gui),
                                                                                                                            _syncObjects(FRAMES_IN_FLIGHT, device),
                                                                                                                            _descriptorSetLayout(layout)
{
    _swapchainImageLayouts.resize(_swapchain->GetSwapchainImages().size(), VK_IMAGE_LAYOUT_UNDEFINED);

    CreateScene();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateBufferRecorder();

    auto subresourceRange = GetImageSubresourceRange();
    auto mapping = GetComponentMapping();
    _imageViews = _swapchain->GetImageViews(subresourceRange, mapping);
}

void SceneDrawer::DrawFrame()
{
    _scene->RequestRebuild();

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

    Update(_currentFrame);

    _recorder->RecordCommandBuffer(_currentFrame, _imageViews[imageIndex], imageIndex);

    _gui->DrawSceneGUI(*_scene);

    VkCommandBuffer guiBuffer = _gui->PrepareCommandBuffer(imageIndex, _imageViews[imageIndex]);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::vector<VkCommandBuffer> commandBuffers;

    commandBuffers.push_back(_commandBuffers[_currentFrame]);
    commandBuffers.push_back(guiBuffer);

    VkSemaphore waitSemaphores[] = {_syncObjects.ImageAvailableSemaphores()[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 2;
    submitInfo.pCommandBuffers = commandBuffers.data();

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

void SceneDrawer::CreateScene()
{
    std::vector<std::shared_ptr<Renderable>> objects;
    //
    std::shared_ptr<Circle> circle1 = std::make_shared<Circle>(0.1, 256);
    // std::shared_ptr<Circle> circle3 = std::make_shared<Circle>(0.1, 256);
    // std::shared_ptr<Circle> circle2 = std::make_shared<Circle>(0.1, 256);
    //
    // std::shared_ptr<Triangle> triangle = std::make_shared<Triangle>();
    // std::shared_ptr<Triangle> triangle2 = std::make_shared<Triangle>();
    // std::shared_ptr<Triangle> triangle3 = std::make_shared<Triangle>();
    // std::shared_ptr<Square> square = std::make_shared<Square>();
    //
    // circle1->UpdateModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.7f, 0.0f)));
    // circle2->UpdateModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.0f, 0.0f)));
    // circle3->UpdateModel(glm::translate(glm::mat4(1.0f), glm::vec3(-0.7f, 0.0f, 0.0f)));
    //
    // triangle->UpdateModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.7f)));
    // triangle2->UpdateModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.7f)));
    // triangle3->UpdateModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.9f)));
    // square->UpdateModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.7f, 0.0f)));
    //
    objects.push_back(circle1);
    // objects.push_back(circle2);
    // objects.push_back(circle3);
    // objects.push_back(triangle);
    // objects.push_back(triangle2);
    // objects.push_back(triangle3);
    // objects.push_back(square);

    _scene = std::make_unique<Scene>(objects, _commandPool, _allocator, _device);
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
        UpdateDescriptorSets(i);
}

void SceneDrawer::UpdateDescriptorSets(uint32_t frameIndex) const
{
    auto storageBuffers = _scene->GetStorageBuffers();

    VkDescriptorBufferInfo bufferInfo {};
    bufferInfo.buffer = storageBuffers[frameIndex]->GetBuffer();
    bufferInfo.buffer = storageBuffers[frameIndex]->GetBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(StorageBufferObject) * _scene->GetBufferObjectsSize();
    VkWriteDescriptorSet descriptorWrite {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = _descriptorSets[frameIndex];
    descriptorWrite.dstBinding = 1;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;
    descriptorWrite.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(_device->GetDevice(), 1, &descriptorWrite, 0, nullptr);
}

void SceneDrawer::Update(uint32_t currentFrame)
{
    UpdateDescriptorSets(currentFrame);
    _recorder.reset();
    CreateBufferRecorder();
    _scene->GetStorageBuffers()[currentFrame]->Update(_scene->GetBufferObjects());
}

void SceneDrawer::CreateBufferRecorder()
{
    SceneCommandBufferRecorderInfo sceneCommandBufferRecorderInfo {};
    sceneCommandBufferRecorderInfo._buffers = &_commandBuffers;
    sceneCommandBufferRecorderInfo._pipeline = _pipeline;
    sceneCommandBufferRecorderInfo._swapchain = _swapchain;
    sceneCommandBufferRecorderInfo._objects = _scene->GetObjectData();
    sceneCommandBufferRecorderInfo._vertexBuffer = _scene->GetVertexBuffer()->GetBuffer();
    sceneCommandBufferRecorderInfo._indexBuffer = _scene->GetIndexBuffer()->GetBuffer();
    sceneCommandBufferRecorderInfo._swapchainImageLayouts = &_swapchainImageLayouts;
    sceneCommandBufferRecorderInfo._descriptorSets = _descriptorSets;

    _recorder = std::make_unique<SceneCommandBufferRecorder>(sceneCommandBufferRecorderInfo);
}

void SceneDrawer::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize {};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    createInfo.maxSets = FRAMES_IN_FLIGHT;

    if (vkCreateDescriptorPool(_device->GetDevice(), &createInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool");
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

SceneDrawer::~SceneDrawer()
{
    for (auto imageView : _imageViews)
        vkDestroyImageView(_device->GetDevice(), imageView, nullptr);

    _scene.reset();

    vkDestroyDescriptorPool(_device->GetDevice(), _descriptorPool, nullptr);

    _recorder.reset();
}