#pragma once

#include "logical-device.h"
#include "../../../file-manager/include/file-reader.h"

class ShaderModule
{

private:

    FileReader _fileReader;

    const LogicalDevice* const _device;

    VkShaderStageFlagBits _stage {};

    std::string _shaderName;

private:

    VkShaderModule _shaderModule {};

    VkPipelineShaderStageCreateInfo _shaderStage {};

public:

    explicit ShaderModule(const LogicalDevice* device, const VkShaderStageFlagBits& stage, const std::string& shaderName);
    ~ShaderModule();

    VkShaderModule GetShaderModule() const { return _shaderModule; }
    VkPipelineShaderStageCreateInfo GetShaderStage() const { return _shaderStage; }

private:

    void CreateShaderModule();
    void CreateShaderStage();

};