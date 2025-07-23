#pragma once

#include "pipeline.h"
#include "shader-module.h"
#include "../../../file-manager/include/file-reader.h"

class GraphicsPipeline : Pipeline
{

private:

    FileReader _fileReader;

    std::vector<ShaderModule*> _shaderModules;

public:

    explicit GraphicsPipeline(const std::vector<ShaderModule*>& shaderModules, const PipelineLayout* pipelineLayout, LogicalDevice* device);

    ~GraphicsPipeline() override;

    VkPipeline GetPipeline() const override { return _pipeline; }

    VkPipelineLayout GetPipelineLayout() const override { return  _pipelineLayout->GetPiplineLayout(); }

private:

    void CreatePipeline() override;

    VkPipelineRasterizationStateCreateInfo CreateRasterizer() const;

    VkPipelineMultisampleStateCreateInfo SetupMultisampling() const;

    VkPipelineColorBlendAttachmentState SetupColorBlendAttachment() const;

    VkPipelineColorBlendStateCreateInfo SetupColorBlending(const VkPipelineColorBlendAttachmentState& attachmentState) const;

};
