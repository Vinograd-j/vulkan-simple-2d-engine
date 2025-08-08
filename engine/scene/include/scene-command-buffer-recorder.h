#pragma once
#include "scene-command-buffer-recorder-info.h"
#include "../../../backend/vulkan/include/image-memory-barrier.h"
#include "../../../backend/vulkan/command-buffer/include/command-buffer-recorder.h"
#include "../../swapchain/include/present-swapchain.h"

class SceneCommandBufferRecorder : CommandBufferRecorder
{

private:

    ImageMemoryBarrier _barrier;

    SceneCommandBufferRecorderInfo _renderingObjects;

public:

    explicit SceneCommandBufferRecorder(const SceneCommandBufferRecorderInfo& info) :
                                                                                        CommandBufferRecorder(info._buffers),
                                                                                        _renderingObjects(info) {}

    ~SceneCommandBufferRecorder() override = default;

    void RecordCommandBuffer(uint32_t bufferIndex, VkImageView imageView, uint32_t imageIndex) const;

    const VkCommandBuffer* GetCommandBuffer(uint32_t index) const { return &_commandBuffers->GetCommandBuffers()[index]; }

};