#pragma once
#include "command-buffers.h"
#include "command-pool.h"

class CommandBufferRecorder
{

protected:

    const CommandPool* const _commandPool;

    CommandBuffers* const _commandBuffers;

public:

    explicit CommandBufferRecorder(const CommandPool* pool, CommandBuffers* commandBuffers) : _commandPool(pool),
                                                                                              _commandBuffers(commandBuffers) {}

    virtual ~CommandBufferRecorder() = default;

    virtual void RecordCommandBuffer(uint32_t bufferIndex, VkImageView imageView, uint32_t imageIndex) = 0;

};
