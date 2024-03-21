#pragma once
#include "Factory.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class ShaderModule : public VulkanObject
    {
        template <VulkanObjectKind T>
        friend class Factory::SubFactory;
        
        ShaderModule(VkShaderModule p_handler, VkShaderStageFlagBits p_stageFlags)
            : _handler(p_handler), _shaderStageFlags(p_stageFlags)
        { }
        
    public:
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
    class Factory::SubFactory<ShaderModule>
    {
    public:
        static Ref<ShaderModule> Create(const std::string& p_filepath, VkShaderStageFlagBits p_stageFlags,
            const Ref<VulkanObject>& p_parent = nullptr);

    private:
        static void Terminate(VulkanObject* p_object);

        static std::vector<char> ReadFile(const std::string& p_filepath);
    };

}
