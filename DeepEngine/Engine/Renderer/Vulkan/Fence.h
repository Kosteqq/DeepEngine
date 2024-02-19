#pragma once
#include "Controller/BaseVulkanController.h"
#include "Debug/VulkanDebug.h"
#include "Instance/VulkanInstance.h"
#include "VulkanFactory.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class Fence final : BaseVulkanController
    {
    public:
        Fence(bool p_signalAtStart = false): _signaledAtStart(p_signalAtStart)
        { }
        ~Fence() override = default;
        
        VkFence GetVkFence() const
        { return _fence; }
        
        const VkFence* GetVkFencePtr() const
        { return &_fence; }
    
    protected:
        bool OnInitialize() override
        {
            VkFenceCreateInfo fenceCreateInfo { };
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    
            if (_signaledAtStart)
            {
                fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            }
    
            VULKAN_CHECK_CREATE(
                vkCreateFence(GetVulkanInstanceController()->GetLogicalDevice(), &fenceCreateInfo, nullptr, &_fence),
                "Failed to create Vulkan Fence")
    
            return true;
        }
    
        void OnTerminate() override
        {
            vkDestroyFence(GetVulkanInstanceController()->GetLogicalDevice(), _fence, nullptr);
        }
    
    private:
        const bool _signaledAtStart = false;
        VkFence _fence = VK_NULL_HANDLE;
    };

    class Fence2 : public VulkanObject
    {
    public:
        Fence2(VkFence p_handler)
            : _handler(p_handler)
        {
            // ...
        }

        VkFence GetHandler() const
        { return _handler; }

    private:
        const VkFence _handler;
    };

    template<>
    class VulkanFactory::SubFactory<Fence2>
    {
    public:
        template <VulkanObjectKind T>
        static VulkanRef<Fence2> Create(VulkanRef<T> p_parent = nullptr)
        {
            VkFenceCreateInfo fenceCreateInfo { };
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            
            VkFence fence;
            vkCreateFence(_bindFactory->_vulkanInstance.GetLogicalDevice(), &fenceCreateInfo, nullptr, &fence);

            auto fenceObject = new Fence2(fence);

            if (p_parent != nullptr)
            {
                return CreateObject(fenceObject, Terminate, p_parent);
            }
            
            return CreateObject(fenceObject, Terminate);
        }

        template <VulkanObjectKind T>
        static VulkanRef<Fence2> CreateSignaled(VulkanRef<T> p_parent = nullptr)
        {
            VkFenceCreateInfo fenceCreateInfo { };
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            
            VkFence fence;
            vkCreateFence(_bindFactory->_vulkanInstance.GetLogicalDevice(), &fenceCreateInfo, nullptr, &fence);

            auto fenceObject = new Fence2(fence);

            if (p_parent != nullptr)
            {
                return CreateObject(fenceObject, Terminate, p_parent);
            }
            
            return CreateObject(fenceObject, Terminate);
        }

    private:
        static void Terminate(VulkanObject* p_object)
        {
            auto fenceObject = (Fence2*)p_object;
            vkDestroyFence(_bindFactory->_vulkanInstance.GetLogicalDevice(), fenceObject->GetHandler(), nullptr);
        }
    };
    
}
