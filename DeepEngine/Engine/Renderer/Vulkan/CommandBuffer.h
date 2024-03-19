#pragma once
#include "CommandPool.h"
#include "Controller/BaseVulkanController.h"

#define _VULKAN_COMMAND_BUFFER_CLASS_DEFINED

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class CommandBuffer final : public BaseVulkanController
    {
    public:
        CommandBuffer(CommandPool* p_commandPool, bool p_createAsSecondary);
        ~CommandBuffer() override = default;

        const VkCommandBuffer& GetVkCommandBuffer() const
        { return _commandBuffer; }

    protected:
        bool OnInitialize() override;
        void OnTerminate() override
        {
            return;
        }

    private:
        VkCommandBuffer _commandBuffer = VK_NULL_HANDLE;

        const CommandPool* _commandPool;
        const bool _isSecondary;
    };

    class CommandBuffer2 : public VulkanObject
    {
    public:
        CommandBuffer2(const VkCommandBuffer p_handler)
            : _handler(p_handler)
        {
            // ...
        }

        VkCommandBuffer GetHandler() const
        {
            return _handler;
        }
        
    private:
        const VkCommandBuffer _handler;
    };

    template <>
    class Factory::SubFactory<CommandBuffer2>
    {
    public:
        static Ref<CommandBuffer2> Create(Ref<CommandPool2> p_commandPool, bool p_asSecondary = false);
        static std::vector<Ref<CommandBuffer2>> CreateMany(Ref<CommandPool2> p_commandPool, uint32_t p_amount, bool p_asSecondary = false);

    private:
        static void Terminate(VulkanObject* p_object);
    };
    
}
