#include "../include/graphics-pipeline.h"

#include "../../struct/vertex.h"

GraphicsPipeline::GraphicsPipeline(const std::vector<ShaderModule*>& shaderModules, const PipelineLayout* pipelineLayout, LogicalDevice* device) : Pipeline(pipelineLayout, device), _shaderModules(shaderModules)
{
    GraphicsPipeline::CreatePipeline();
}

void GraphicsPipeline::CreatePipeline()
{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    for (int i = 0; i < _shaderModules.size(); ++i)
        shaderStages.push_back(_shaderModules[i]->GetShaderStage());

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    std::vector dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = CreateRasterizer();

    VkPipelineMultisampleStateCreateInfo multisampling = SetupMultisampling();

    VkPipelineColorBlendAttachmentState colorBlendAttachment = SetupColorBlendAttachment();

    VkPipelineColorBlendStateCreateInfo colorBlending = SetupColorBlending(colorBlendAttachment);

    VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
    VkPipelineRenderingCreateInfo rendering {};
    rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering.colorAttachmentCount = 1;
    rendering.pColorAttachmentFormats = &colorFormat;
    rendering.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    rendering.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

    VkGraphicsPipelineCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = shaderStages.size();
    createInfo.pStages = shaderStages.data();
    createInfo.pVertexInputState = &vertexInputInfo;
    createInfo.pInputAssemblyState = &inputAssembly;
    createInfo.pViewportState = &viewportState;
    createInfo.pMultisampleState = &multisampling;
    createInfo.pDepthStencilState = nullptr;
    createInfo.pColorBlendState = &colorBlending;
    createInfo.pDynamicState = &dynamicState;
    createInfo.pRasterizationState = &rasterizer;
    createInfo.layout = _pipelineLayout->GetPiplineLayout();
    createInfo.renderPass = nullptr;
    createInfo.subpass = 0;
    createInfo.pNext = &rendering;

    if (vkCreateGraphicsPipelines(_device->GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &_pipeline) != VK_SUCCESS)
        throw std::runtime_error("failed to create graphics pipeline");
}


VkPipelineRasterizationStateCreateInfo GraphicsPipeline::CreateRasterizer() const
{
    VkPipelineRasterizationStateCreateInfo rasterizer {};

    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthBiasClamp = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasClamp = VK_FALSE;

    return rasterizer;
}

VkPipelineMultisampleStateCreateInfo GraphicsPipeline::SetupMultisampling() const
{
    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    return multisampling;
}

VkPipelineColorBlendAttachmentState GraphicsPipeline::SetupColorBlendAttachment() const
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo GraphicsPipeline::SetupColorBlending(const VkPipelineColorBlendAttachmentState& attachmentState) const
{
    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &attachmentState;

    return colorBlending;
}

GraphicsPipeline::~GraphicsPipeline()
{
    vkDestroyPipeline(_device->GetDevice(), _pipeline, nullptr);
}