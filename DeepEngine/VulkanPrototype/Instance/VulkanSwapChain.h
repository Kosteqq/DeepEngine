#pragma once
#include <cstdint>
#include <limits>
#include <algorithm>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "VulkanLogicalLayer.h"
#include "VulkanPhysicalLayer.h"

namespace DeepEngine::Renderer
{
    class VulkanSwapChain
    {
    public:
        VulkanSwapChain(std::shared_ptr<Core::Debug::Logger> p_logger,
            const VulkanPhysicalLayer* p_physicalLayer, const VulkanLogicalLayer* p_logicalLayer,
            const VkSurfaceKHR& p_surface, uint32_t p_width, uint32_t p_height)
            : _logger(p_logger), _physicalLayer(p_physicalLayer), _logicalLayer(p_logicalLayer),
                _surface(p_surface), _width(p_width), _height(p_height)
        { }

        bool Init()
        {
            const VkPhysicalDevice& device = _physicalLayer->GetDevice();
            
            VkSurfaceCapabilitiesKHR capabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &capabilities);

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

            if (formatCount == 0 || presentModeCount == 0)
            {
                return false;
            }
            
            auto availableFormats = std::vector<VkSurfaceFormatKHR>(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalLayer->GetDevice(), _surface, &formatCount, availableFormats.data());

            auto availablePresentModes = std::vector<VkPresentModeKHR>(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, availablePresentModes.data());

            _format = GetBestFormat(availableFormats); 
            _present = GetBestPresentMode(availablePresentModes);
            _extent = GetSwapExtent(capabilities);

            uint32_t minFramesCount = capabilities.minImageCount;
            // If max frames is 0, there is no limit
            uint32_t maxFramesCount = capabilities.maxImageCount;

            uint32_t framesCount = minFramesCount + 1;

            VkSwapchainCreateInfoKHR createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = _surface;

            createInfo.minImageCount = framesCount;
            createInfo.imageFormat = _format.format;
            createInfo.imageColorSpace = _format.colorSpace;
            createInfo.imageExtent = _extent;
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

            createInfo.preTransform = capabilities.currentTransform;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode = _present;
            createInfo.clipped = VK_TRUE;
            createInfo.oldSwapchain = VK_NULL_HANDLE;

            
            VkResult result = vkCreateSwapchainKHR(_logicalLayer->GetLogicalDevice(), &createInfo, nullptr, &_swapChain);
            if (result != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Faied to create swap chain with result: {}", string_VkResult(result));
                return false;
            }

            uint32_t imagesCount = 0;
            vkGetSwapchainImagesKHR(_logicalLayer->GetLogicalDevice(), _swapChain, &imagesCount, nullptr);
            _swapChainImages.resize(imagesCount);
            vkGetSwapchainImagesKHR(_logicalLayer->GetLogicalDevice(), _swapChain, &imagesCount, _swapChainImages.data());
            
            return true;
        }

        void Terminate()
        {
            vkDestroySwapchainKHR(_logicalLayer->GetLogicalDevice(), _swapChain, nullptr);

            for (int i = 0; i < _swapChainImageViews.size(); i++)
            {
                vkDestroyImageView(_logicalLayer->GetLogicalDevice(), _swapChainImageViews[i], nullptr);
            }
        }

        VkSurfaceFormatKHR GetBestFormat(std::vector<VkSurfaceFormatKHR> p_availableFormats)
        {
            for (uint32_t i = 0; i < p_availableFormats.size(); i++)
            {
                if (p_availableFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB
                    && p_availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return p_availableFormats[i];
                }
            }

            return p_availableFormats[0];
        }

        VkPresentModeKHR GetBestPresentMode(std::vector<VkPresentModeKHR> p_availablePresentModes)
        {
            for (uint32_t i = 0; i < p_availablePresentModes.size(); i++)
            {
                if (p_availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    return VK_PRESENT_MODE_MAILBOX_KHR;
                }
                
            }

            return p_availablePresentModes[0];
        }

        VkExtent2D GetSwapExtent(const VkSurfaceCapabilitiesKHR& p_capabilities)
        {
            return VkExtent2D
            {
                std::clamp(_width, p_capabilities.minImageExtent.width, p_capabilities.maxImageExtent.width),
                std::clamp(_height, p_capabilities.minImageExtent.height, p_capabilities.maxImageExtent.height),
            };
        }

        bool CreateSwapChainImageViews()
        {
            _swapChainImageViews.resize(_swapChainImages.size());
            for (uint32_t i = 0; i < _swapChainImageViews.size(); i++)
            {
                VkImageViewCreateInfo createInfo { };
                createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                createInfo.image = _swapChainImages[i];
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.format = _format.format;

                createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

                createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                createInfo.subresourceRange.baseMipLevel = 0;
                createInfo.subresourceRange.levelCount = 1;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.layerCount = 1;

                const auto result = vkCreateImageView(_logicalLayer->GetLogicalDevice(), &createInfo, nullptr, &_swapChainImageViews[i]);
                if (result != VK_SUCCESS)
                {
                    LOG_ERR(_logger, "Faied to swapchain image view[{}]: {}", i, string_VkResult(result));
                }
            }
        }

        VkExtent2D GetExtent() const
        { return _extent; }

    private:
        uint32_t _width;
        uint32_t _height;
        VkSwapchainKHR _swapChain;
        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;

        VkSurfaceFormatKHR _format;
        VkPresentModeKHR _present;
        VkExtent2D _extent;

        std::shared_ptr<Core::Debug::Logger> _logger;
        const VulkanPhysicalLayer* _physicalLayer;
        const VulkanLogicalLayer* _logicalLayer;
        const VkSurfaceKHR& _surface;
        
    };
}
