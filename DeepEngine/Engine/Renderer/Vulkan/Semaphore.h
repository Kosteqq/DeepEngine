#pragma once
#include "Factory.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class Semaphore : public VulkanObject
    {
        template <VulkanObjectKind T>
        friend class Factory::SubFactory;
        
        Semaphore(VkSemaphore p_handler) : _handler(p_handler)
        { }
        
    public:
        VkSemaphore GetHandler() const
        { return _handler; }
        
        const VkSemaphore _handler;
    };
    
    template<>
    class Factory::SubFactory<Semaphore>
    {
    public:
        static Ref<Semaphore> Create(const Ref<VulkanObject>& p_parent = nullptr);

    private:
        static void Terminate(VulkanObject* p_object);
    };
    
}
