#pragma once
#include "Controller/BaseVulkanController.h"
#include "Debug/VulkanDebug.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{

    class VulkanFence final : BaseVulkanController
    {
        friend VulkanFence* CreateVulkanSubController(BaseVulkanController* p_parent);

    private:
        ~VulkanFence() override = default;
        
    public:
        VulkanFence() = default;
        
        VkFence& GetFence()
        { return _fence; }

    protected:
        bool OnInitialize() override
        {
            VkFenceCreateInfo fenceCreateInfo { };
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

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
        VkFence _fence = VK_NULL_HANDLE;
    };
}
