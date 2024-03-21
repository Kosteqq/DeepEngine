#pragma once
#include "CommandPool.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class CommandBuffer : public VulkanObject
    {
        template <VulkanObjectKind T>
        friend class Factory::SubFactory;
        
        CommandBuffer(const VkCommandBuffer p_handler)
            : _handler(p_handler)
        {
            // ...
        }

    public:
        VkCommandBuffer GetHandler() const
        { return _handler; }

        const VkCommandBuffer* GetPtr() const
        { return &_handler; }
        
    private:
        const VkCommandBuffer _handler;
    };

    template <>
    class Factory::SubFactory<CommandBuffer>
    {
    public:
        static Ref<CommandBuffer> Create(Ref<CommandPool> p_commandPool, bool p_asSecondary = false);
        static std::vector<Ref<CommandBuffer>> CreateMany(Ref<CommandPool> p_commandPool, uint32_t p_amount, bool p_asSecondary = false);

    private:
        static void Terminate(VulkanObject* p_object);
    };
    
}
