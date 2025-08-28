#include "../include/scene-drawer.h"

#include <chrono>
#include <imgui_impl_vulkan.h>
#include <iostream>
#include <random>
#include <glm/ext/matrix_transform.hpp>

#include "../../../backend/vulkan/buffers/include/storage-buffer.h"
#include "../../../backend/vulkan/buffers/include/view-projection-buffer.h"
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

    if (_scene->GetBufferObjectsSize() != 0)
        _recorder->RecordCommandBuffer(_currentFrame, _imageViews[imageIndex], imageIndex);

    _gui->DrawSceneGUI(*_scene);

    VkCommandBuffer guiBuffer = _gui->PrepareCommandBuffer(imageIndex, _imageViews[imageIndex]);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::vector<VkCommandBuffer> commandBuffers;

    if (_scene->GetBufferObjectsSize() != 0)
        commandBuffers.push_back(_commandBuffers[_currentFrame]);
    commandBuffers.push_back(guiBuffer);

    VkSemaphore waitSemaphores[] = {_syncObjects.ImageAvailableSemaphores()[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = commandBuffers.size();
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
    _scene = std::make_unique<Scene>(_commandPool, _allocator, _device);
    _scene->AddObject(std::make_shared<Circle>(0.3, 128));
}

void SceneDrawer::CreateDescriptorSets()
{
    std::vector layouts(FRAMES_IN_FLIGHT, _descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    _descriptorSets.resize(FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(_device->GetDevice(), &allocInfo, _descriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets");

    if (_scene->GetBufferObjectsSize() == 0) return;

    for (int i = 0; i < FRAMES_IN_FLIGHT; ++i)
        UpdateDescriptorSets(i);
}

void SceneDrawer::UpdateDescriptorSets(uint32_t frameIndex) const
{
    auto storageBuffers = _scene->GetStorageBuffers();

    if (storageBuffers.size() == 0) return;

    VkDescriptorBufferInfo bufferInfo {};
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

    VkDescriptorBufferInfo vpbInfo {};
    vpbInfo.buffer = _scene->GetViewProjectionBuffers()[frameIndex]->GetBuffer();
    vpbInfo.offset = 0;
    vpbInfo.range = sizeof(ViewProjectionBuffer);
    VkWriteDescriptorSet descriptorWriteVPB {};
    descriptorWriteVPB.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWriteVPB.dstSet = _descriptorSets[frameIndex];
    descriptorWriteVPB.dstBinding = 0;
    descriptorWriteVPB.dstArrayElement = 0;
    descriptorWriteVPB.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWriteVPB.descriptorCount = 1;
    descriptorWriteVPB.pBufferInfo = &vpbInfo;
    descriptorWriteVPB.pImageInfo = nullptr;
    descriptorWriteVPB.pTexelBufferView = nullptr;

    std::array writes { descriptorWrite, descriptorWriteVPB };

    vkUpdateDescriptorSets(_device->GetDevice(), writes.size(), writes.data(), 0, nullptr);
}

void SceneDrawer::Update(uint32_t currentFrame)
{
    _recorder.reset();
    CreateBufferRecorder();

    auto& viewProj = _scene->GetViewProjectionBuffers();

    float aspectRatio = static_cast<float>(_swapchain->GetExtent().width) / _swapchain->GetExtent().height;
    glm::mat4 aspectFix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / aspectRatio, 1.0f, 1.0f));
    ViewProjectionBuffer vp{};
    vp._view = glm::mat4(1.0f);
    vp._proj = aspectFix;

    viewProj[currentFrame]->Update(vp);

    _scene->GetStorageBuffers()[currentFrame]->Update(_scene->GetBufferObjects());

    UpdateDescriptorSets(currentFrame);
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