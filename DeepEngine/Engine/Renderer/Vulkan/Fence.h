#pragma once
#include "Instance/VulkanInstance.h"
#include "Factory.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class Fence : public VulkanObject
    {
        template <VulkanObjectKind T>
        friend class Factory::SubFactory;
        
        Fence(VkFence p_handler)
            : _handler(p_handler)
        {
            // ...
        }

    public:
        VkFence GetHandler() const
        { return _handler; }

        const VkFence* GetPtr() const
        { return &_handler; }

    private:
        const VkFence _handler;
    };

    template<>
    class Factory::SubFactory<Fence>
    {
    public:
        static Ref<Fence> Create(const Ref<VulkanObject>& p_parent = nullptr);
        static Ref<Fence> CreateSignaled(const Ref<VulkanObject>& p_parent = nullptr);

    private:
        static void Terminate(VulkanObject* p_object);
    };
    
}
