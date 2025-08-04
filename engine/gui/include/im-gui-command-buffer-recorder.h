#pragma once
#include "../../vulkan/command-buffer/include/command-buffer-recorder.h"

class ImGUICommandBufferRecorder : CommandBufferRecorder
{

private:

    VkExtent2D _extent;

public:

    explicit ImGUICommandBufferRecorder(CommandBuffers* commandBuffers, const VkExtent2D& extent) :
                                                                                                    CommandBufferRecorder(commandBuffers),
                                                                                                    _extent(extent){}

    ~ImGUICommandBufferRecorder() override = default;

    void RecordCommandBuffer(uint32_t bufferIndex, VkImageView imageView) const;

};
