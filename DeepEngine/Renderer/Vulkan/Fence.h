#pragma once
#include "Controller/BaseVulkanController.h"
#include "Debug/VulkanDebug.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
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
}
