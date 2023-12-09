#include "VulkanShaderModule.h"

#include <fstream>

namespace DeepEngine::Renderer::Vulkan
{
    VulkanShaderModule::VulkanShaderModule(const std::string& p_filepath, VkShaderStageFlagBits p_shaderStage)
        : _filepath(p_filepath), _shaderStageFlags(p_shaderStage)
    { }

    bool VulkanShaderModule::OnInitialize()
    {
        const auto shaderCode = ReadFile();

        if (shaderCode.empty())
        {
            VULKAN_ERR("Failed to load shader from filepath: \"{}\". Loaded files size {}", _filepath, shaderCode.size());
            return false;
        }
            
        VkShaderModuleCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        VULKAN_CHECK_CREATE(
            vkCreateShaderModule(
                GetVulkanInstanceController()->GetLogicalDevice(),
                &createInfo,
                nullptr,
                &_shaderModule),
            "Failed to create Vulkan Shader Module!")
            
        return true;
    }

    void VulkanShaderModule::OnTerminate()
    {
        vkDestroyShaderModule(GetVulkanInstanceController()->GetLogicalDevice(), _shaderModule, nullptr);
    }

    std::vector<char> DeepEngine::Renderer::Vulkan::VulkanShaderModule::ReadFile() const
    {
        std::ifstream file(_filepath, std::ios::ate | std::ios::binary);

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
}
