#include "../include/im-gui-command-buffer-recorder.h"

void ImGUICommandBufferRecorder::RecordCommandBuffer(uint32_t bufferIndex, VkImageView imageView) const
{
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(_commandBuffers->GetCommandBuffers()[bufferIndex], &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin command buffer");

    VkRenderingAttachmentInfoKHR colorAttachment {};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    colorAttachment.imageView = imageView;
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachment.resolveImageView = VK_NULL_HANDLE;
    colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfoKHR renderingInfo {};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = _extent;
    renderingInfo.pColorAttachments = &colorAttachment;

    vkCmdBeginRendering(_commandBuffers->GetCommandBuffers()[bufferIndex], &renderingInfo);
}