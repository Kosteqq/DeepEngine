#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{
    class GraphicsPipeline;

    class ShaderModule final : public BaseVulkanController
    {
    public:
        ShaderModule(const std::string& p_filepath, VkShaderStageFlagBits p_shaderStage);
        ~ShaderModule() override = default;

        VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const
        {
            VkPipelineShaderStageCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            createInfo.stage = _shaderStageFlags;
            createInfo.module = _shaderModule;
            createInfo.pName = _shaderFuncName.c_str();
            return createInfo;
        }

        VkShaderModule GetShaderModule() const
        { return  _shaderModule; }

        const std::string& GetShaderFuncName() const
        { return  _shaderFuncName; }

        VkShaderStageFlagBits GetShaderStageFlags() const
        { return  _shaderStageFlags; }
        
    protected:
        bool OnInitialize() final;
        void OnTerminate() final;

    private:
        std::vector<char> ReadFile() const;

    private:
        const std::string _filepath;
        const VkShaderStageFlagBits _shaderStageFlags;
        
        const std::string _shaderFuncName = "main";
        VkShaderModule _shaderModule = VK_NULL_HANDLE;
    };

}
