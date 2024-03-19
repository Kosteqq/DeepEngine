#pragma once
#include "Instance/VulkanInstance.h"
#include "VulkanFactory.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class Fence : public VulkanObject
    {
    public:
        Fence(VkFence p_handler)
            : _handler(p_handler)
        {
            // ...
        }

        VkFence GetHandler() const
        { return _handler; }

        const VkFence* GetPtr() const
        { return &_handler; }

    private:
        const VkFence _handler;
    };

    template<>
    class VulkanFactory::SubFactory<Fence>
    {
    public:
        template <VulkanObjectKind T>
        static Ref<Fence> Create(Ref<T> p_parent = nullptr)
        {
            VkFenceCreateInfo fenceCreateInfo { };
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            
            VkFence fence;
            vkCreateFence(_bindFactory->_vulkanInstance.GetLogicalDevice(), &fenceCreateInfo, nullptr, &fence);

            auto fenceObject = new Fence(fence);

            return CreateObject(fenceObject, Terminate, p_parent);
        }

        template <VulkanObjectKind T = VulkanObject>
        static Ref<Fence> CreateSignaled(Ref<T> p_parent = nullptr)
        {
            VkFenceCreateInfo fenceCreateInfo { };
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            
            VkFence fence;
            vkCreateFence(_bindFactory->_vulkanInstance.GetLogicalDevice(), &fenceCreateInfo, nullptr, &fence);

            auto fenceObject = new Fence(fence);

            return CreateObject(fenceObject, Terminate, p_parent);
        }

    private:
        static void Terminate(VulkanObject* p_object)
        {
            auto fenceObject = (Fence*)p_object;
            vkDestroyFence(_bindFactory->_vulkanInstance.GetLogicalDevice(), fenceObject->GetHandler(), nullptr);
        }
    };
    
}
