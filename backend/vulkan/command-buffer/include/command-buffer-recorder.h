#pragma once

#include "command-buffers.h"

class CommandBufferRecorder
{

protected:

    CommandBuffers* const _commandBuffers;

public:

    explicit CommandBufferRecorder(CommandBuffers* commandBuffers) :
                                                                     _commandBuffers(commandBuffers) {}

    virtual ~CommandBufferRecorder() = default;

};