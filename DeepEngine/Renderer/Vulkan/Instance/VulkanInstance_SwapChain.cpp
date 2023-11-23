#include "VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{
    bool VulkanInstance::OnInitializeSwapChain()
    {
        uint32_t minFramesCount = _availableSurfaceCapabilities.minImageCount;
        // If max frames is 0, there is no limit
        uint32_t maxFramesCount = _availableSurfaceCapabilities.maxImageCount;

        uint32_t framesCount = minFramesCount + 1;

        const auto& capabilities = _availableSurfaceCapabilities; 
        VkExtent2D clampedBufferExtent {
            std::clamp((uint32_t)_swapChainCurrentFrameBufferSize.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp((uint32_t)_swapChainCurrentFrameBufferSize.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
        };

        VkSwapchainCreateInfoKHR createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _surface;

        createInfo.minImageCount = framesCount;
        createInfo.imageFormat = _swapchainCurrentFormat.format;
        createInfo.imageColorSpace = _swapchainCurrentFormat.colorSpace;
        createInfo.imageExtent = clampedBufferExtent;
        createInfo.imageArrayLayers = 1; // more is for e.g. stereoscopic 3D
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // For multiple queues
        // createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        // createInfo.queueFamilyIndexCount = 2;
        // createInfo.pQueueFamilyIndices = ;

        // For single queue -> better performance
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // optional
        createInfo.pQueueFamilyIndices = nullptr; // optional

        createInfo.preTransform = _availableSurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = _swapChainCurrentPresentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VULKAN_CHECK_CREATE(
            vkCreateSwapchainKHR(_logicalDevice, &createInfo, nullptr, &_swapchain),
            "Failed to create SwapChain!")

        uint32_t imagesCount = 0;
        vkGetSwapchainImagesKHR(_logicalDevice, _swapchain, &imagesCount, nullptr);
        _swapChainImages.resize(imagesCount);
        _swapChainImageViews.resize(imagesCount);

        vkGetSwapchainImagesKHR(_logicalDevice, _swapchain, &imagesCount, _swapChainImages.data());
        
        for (uint32_t i = 0; i < imagesCount; i++)
        {
            VkImageViewCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = _swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = _swapchainCurrentFormat.format;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VULKAN_CHECK_CREATE(
                vkCreateImageView(_logicalDevice, &createInfo, nullptr, &_swapChainImageViews[i]),
                "Failed to create SwapChain image view!")
        }

        _isSwapChainValid = true;
        
        // TODO (Kostek): Move this code to place like RenderPassController
        // for (uint32_t i = 0; i < imagesCount; i++)
        // {
        //     VkImageView attachments[] { _swapChainImageViews[i] };
        //         
        //     VkFramebufferCreateInfo createInfo { };
        //     createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        //     createInfo.renderPass = _renderPass->GetVulkanRenderPass();
        //     createInfo.attachmentCount = 1;
        //     createInfo.pAttachments = attachments;
        //     createInfo.width = _currentSwapChainFrameBufferSize.x;
        //     createInfo.height = _currentSwapChainFrameBufferSize.y;
        //     createInfo.layers = 1;
        //
        //     VULKAN_CHECK_CREATE(
        //         vkCreateFramebuffer(_logicalDevice, &createInfo, nullptr, &_swapChainFramebuffers[i]),
        //         "Failed to create SwapChain image view!")
        // }
    }

    void VulkanInstance::TerminateSwapChain()
    {
        for (int i = 0; i < _swapChainImageViews.size(); i++)
        {
            vkDestroyImageView(_logicalDevice, _swapChainImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(_logicalDevice, _swapchain, nullptr);
    }

    void VulkanInstance::RecreateSwapChain()
    {
        vkDeviceWaitIdle(_logicalDevice);

        TerminateSwapChain();
        OnInitializeSwapChain();
    }
    
    void VulkanInstance::SetSwapChainFormat(VkSurfaceFormatKHR p_format, VkPresentModeKHR p_presentMode, bool p_ignoreRecreate)
    {
        _swapchainCurrentFormat = p_format;
        _swapChainCurrentPresentMode = p_presentMode;
        _isSwapChainValid = false;
        
        if (!p_ignoreRecreate)
        {
            RecreateSwapChain();
        }
    }
}
