#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{

    class Semaphore final : public BaseVulkanController
    {
    public:
        Semaphore() = default;
        ~Semaphore() override = default;

        const VkSemaphore& GetVkSemaphore() const
        { return _semaphore; }

    protected:
        bool OnInitialize() override
        {
            VkSemaphoreCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            createInfo.flags = 0;

            VULKAN_CHECK_CREATE(
                vkCreateSemaphore(
                    GetVulkanInstanceController()->GetLogicalDevice(),
                    &createInfo,
                    nullptr,
                    &_semaphore),
                "Failed to create Vulkan Semaphore!")

            return true;
        }
        
        void OnTerminate() override
        {
            vkDestroySemaphore(GetVulkanInstanceController()->GetLogicalDevice(), _semaphore, nullptr);
        }

    private:
        VkSemaphore _semaphore;
    };
    
}
