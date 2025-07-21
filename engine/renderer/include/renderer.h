#pragma once

#include "../../pipeline/include/graphics-pipeline.h"
#include "../../swapchain/include/present-swapchain.h"

class Renderer
{

protected:

    const GraphicsPipeline* _pipeline;

    PresentSwapchain* _swapchain;

    const LogicalDevice* _device;

public:

    explicit Renderer(const GraphicsPipeline* pipeline, PresentSwapchain* swapchain, const LogicalDevice* device) :

                                                                                                                   _pipeline(pipeline),
                                                                                                                   _swapchain(swapchain),
                                                                                                                   _device(device) {}

    virtual ~Renderer() = default;

    virtual void DrawFrame() = 0;

};