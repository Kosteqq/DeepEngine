#include "ShaderModule.h"

#include <fstream>

namespace DeepEngine::Engine::Renderer::Vulkan
{

    Ref<ShaderModule> Factory::SubFactory<ShaderModule>::Create(const std::string& p_filepath,
        VkShaderStageFlagBits p_stageFlags, const Ref<VulkanObject>& p_parent)
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
        VULKAN_ASSERT_RESULT_V(
            vkCreateShaderModule(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &createInfo,
                nullptr,
                &handler),
            nullptr)

        return CreateObject(new ShaderModule(handler, p_stageFlags), Terminate, p_parent);
    }

    void Factory::SubFactory<ShaderModule>::Terminate(VulkanObject* p_object)
    {
        vkDestroyShaderModule(
            _bindFactory->_vulkanInstance.GetLogicalDevice(),
            ((ShaderModule*)p_object)->GetHandler(),
            nullptr);
    }

    std::vector<char> Factory::SubFactory<ShaderModule>::ReadFile(const std::string& p_filepath)
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
}
