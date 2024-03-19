#pragma once
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class Semaphore : public VulkanObject
    {
    public:
        Semaphore(VkSemaphore p_handler) : _handler(p_handler)
        { }
        
        VkSemaphore GetHandler() const
        { return _handler; }
        
        const VkSemaphore _handler;
    };
    
    template<>
    class VulkanFactory::SubFactory<Semaphore>
    {
    public:
        template <VulkanObjectKind TParent>
        static Ref<Semaphore> Create(const Ref<TParent>& p_parent)
        {
            VkSemaphoreCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            createInfo.flags = 0;

            VkSemaphore handler;
            vkCreateSemaphore(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &createInfo,
                nullptr,
                &handler);

            return CreateObject(new Semaphore(handler), Terminate, p_parent);
        }
        
        static Ref<Semaphore> Create()
        {
            VkSemaphoreCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            createInfo.flags = 0;
        
            VkSemaphore handler;
            vkCreateSemaphore(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &createInfo,
                nullptr,
                &handler);
        
            return CreateObject<Semaphore, VulkanObject>(new Semaphore(handler), Terminate);
        }

        static void Terminate(VulkanObject* p_object)
        {
            vkDestroySemaphore(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                ((Semaphore*)p_object)->GetHandler(),
                nullptr);
        }
    };
    
}
