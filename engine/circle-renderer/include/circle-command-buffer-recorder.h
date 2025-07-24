#pragma once
#include "circle-command-buffer-recorder-info.h"
#include "../../renderer/command-buffer/include/command-buffer-recorder.h"
#include "../../renderer/include/image-memory-barrier.h"
#include "../../swapchain/include/present-swapchain.h"

class CircleCommandBufferRecorder : CommandBufferRecorder
{

private:

    ImageMemoryBarrier _barrier;

    CircleCommandBufferRecorderInfo _renderingObjects;

public:

    explicit CircleCommandBufferRecorder(const CircleCommandBufferRecorderInfo& info) :
                                                                                        CommandBufferRecorder(info.pool, info.buffers),
                                                                                        _renderingObjects(info) {}

    ~CircleCommandBufferRecorder() override = default;

    void RecordCommandBuffer(uint32_t bufferIndex, VkImageView imageView, uint32_t imageIndex) override;

    const VkCommandBuffer* GetCommandBuffer(uint32_t index) const { return &_commandBuffers->GetCommandBuffer()[index]; }

};