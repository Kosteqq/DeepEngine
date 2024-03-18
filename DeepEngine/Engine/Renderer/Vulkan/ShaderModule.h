#pragma once
#include <fstream>

#include "VulkanFactory.h"
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
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

    class ShaderModule2 : public VulkanObject
    {
    public:
        ShaderModule2(VkShaderModule p_handler, VkShaderStageFlagBits p_stageFlags)
            : _handler(p_handler), _shaderStageFlags(p_stageFlags)
        { }
        
        VkShaderModule GetHandler() const
        { return _handler; }

        const char* GetFuncName() const
        { return "main"; }

        VkShaderStageFlagBits GetStageFlag() const
        { return _shaderStageFlags; }

    private:
        const VkShaderModule _handler;
        const VkShaderStageFlagBits _shaderStageFlags;
    };

    template <>
    class VulkanFactory::SubFactory<ShaderModule2>
    {
    public:
        static Ref<ShaderModule2> Create(const std::string& p_filepath, VkShaderStageFlagBits p_stageFlags,
            const Ref<VulkanObject>& p_parent = nullptr)
        {
            const auto shaderCode = ReadFile(p_filepath);

            if (shaderCode.empty())
            {
                VULKAN_ERR("Failed to load shader from filepath: \"{}\"!!", p_filepath);
                return nullptr;
            }
            
            VkShaderModuleCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = shaderCode.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

            VkShaderModule handler;
            
            vkCreateShaderModule(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &createInfo,
                nullptr,
                &handler);

            return CreateObject(new ShaderModule2(handler, p_stageFlags), Terminate, p_parent);
        }
        
    private:
        static void Terminate(VulkanObject* p_object)
        {
            vkDestroyShaderModule(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                ((ShaderModule2*)p_object)->GetHandler(),
                nullptr);
        }

        static std::vector<char> ReadFile(const std::string& p_filepath)
        {
            std::ifstream file(p_filepath, std::ios::ate | std::ios::binary);

            if (!file.is_open())
            {
                return std::vector<char> { };
            }

            size_t fileSize = file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();

            return buffer;
        }
    };

}
