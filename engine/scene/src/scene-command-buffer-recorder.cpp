#include "../include/scene-command-buffer-recorder.h"

#include <iostream>

void SceneCommandBufferRecorder::RecordCommandBuffer(uint32_t bufferIndex, VkImageView imageView, uint32_t imageIndex) const
{
    auto& layouts = *_renderingObjects._swapchainImageLayouts;

    VkCommandBuffer buffer = _commandBuffers->GetCommandBuffers()[bufferIndex];

    vkResetCommandBuffer(buffer, 0);

    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin command buffer");

    VkClearValue clearColor = {0.0, 0.0, 0.0};

    VkRenderingAttachmentInfo colorAttachmentInfo{};
    colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachmentInfo.imageView = imageView;
    colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.clearValue = clearColor;

    VkRenderingInfo renderingInfo {};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = _renderingObjects._swapchain->GetExtent();
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachmentInfo;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(_renderingObjects._swapchain->GetExtent().width);
    viewport.height = static_cast<float>(_renderingObjects._swapchain->GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = _renderingObjects._swapchain->GetExtent();
    vkCmdSetScissor(buffer, 0, 1, &scissor);

    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _renderingObjects._pipeline->GetPipeline());

    auto barrierToRender = _barrier.CreateBarrier
    (
        _renderingObjects._swapchain->GetSwapchainImages()[imageIndex],
        layouts[imageIndex],
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    barrierToRender.srcAccessMask = 0;
    barrierToRender.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    layouts[imageIndex] = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    vkCmdPipelineBarrier(
        buffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrierToRender
    );

    vkCmdBeginRendering(buffer, &renderingInfo);

    vkCmdBindDescriptorSets(
        buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _renderingObjects._pipeline->GetPipelineLayout(),
        0,
        1,
        &_renderingObjects._descriptorSets[bufferIndex],
        0,
        nullptr
    );

    VkBuffer vertexBuffers[] = { _renderingObjects._vertexBuffer };

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(buffer, _renderingObjects._indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    for (size_t i = 0; i < _renderingObjects._objects.size(); ++i)
    {
        const auto& obj = _renderingObjects._objects[i];

        vkCmdPushConstants(buffer, _renderingObjects._pipeline->GetPipelineLayout(),
                   VK_SHADER_STAGE_VERTEX_BIT, 0,
                   sizeof(uint32_t), &i);

        vkCmdDrawIndexed
        (
            buffer,
            obj._indexCount,
            1,
            obj._indexOffset,
            0,
            1
        );

    }

    vkCmdEndRendering(buffer);

    auto barrierToPresent = _barrier.CreateBarrier
    (
        _renderingObjects._swapchain->GetSwapchainImages()[imageIndex],
        layouts[imageIndex],
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    );

    barrierToPresent.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrierToPresent.dstAccessMask = 0;

    layouts[imageIndex] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    vkCmdPipelineBarrier(
        buffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrierToPresent
    );

    vkEndCommandBuffer(buffer);
}