#pragma once

#include <vulkan/vulkan_core.h>

#include "logical-device.h"
#include "pipeline-layout.h"

class Pipeline
{

protected:

    VkPipeline _pipeline {};

    const PipelineLayout* const _pipelineLayout;

protected:

    LogicalDevice* _device;

public:

    explicit Pipeline(const PipelineLayout* pipelineLayout, LogicalDevice* device) : _pipelineLayout(pipelineLayout), _device(device) {}

    virtual ~Pipeline() = default;

public:

    virtual VkPipeline GetPipeline() const = 0;

    virtual VkPipelineLayout GetPipelineLayout() const = 0;

private:

    virtual void CreatePipeline() = 0;

};