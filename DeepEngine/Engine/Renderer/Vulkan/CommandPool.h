#pragma once
#include "Factory.h"
#include "VulkanObject.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    enum CommandPoolFlag
    {
        TRANSIENT            = 1 << 0,
        RESET_COMMAND_BUFFER = 1 << 1,
        PROTECTED            = 1 << 2,
    };

    class CommandPool : public VulkanObject
    {
        template <VulkanObjectKind T>
        friend class Factory::SubFactory;
        
        CommandPool(VkCommandPool p_handler, const VulkanInstance::QueueInstance* p_queue)
                : _handler(p_handler), _queue(p_queue)
        {
            // ...
        }

    public:
        VkCommandPool GetHandler() const
        {
            return _handler;
        }
        
    private:
        const VkCommandPool _handler;
        const VulkanInstance::QueueInstance* _queue;
    };

    template <>
    class Factory::SubFactory<CommandPool>
    {
    public:
        static Ref<CommandPool> Create(const VulkanInstance::QueueInstance* p_queue,
            CommandPoolFlag p_flags, Ref<VulkanObject> p_parent = nullptr);

    private:
        static void Terminate(VulkanObject* p_object);
    };
    
}
