#pragma once
#include "Vulkan/RenderPass.h"
#include "Vulkan/PipelineLayout.h"
#include "Vulkan/Events/VulkanEvents.h"
#include "Renderer/Events.h"

namespace DeepEngine::Renderer
{

    BEGIN_LOCAL_EVENT_DEFINITION(MainRenderPassRecreatedAttachment)
    END_EVENT_DEFINITION

    class MainRenderPass : public Vulkan::RenderPass
    {
    protected:
        void Initialize() override
        {
            auto vulkanController = GetVulkanInstanceController();
            
            VkAttachmentDescription baseColorAttachmentDesc { };
            baseColorAttachmentDesc.format = VK_FORMAT_R8G8B8A8_SRGB;
            baseColorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
            baseColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            baseColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            baseColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            baseColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            baseColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            baseColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            
            VkSubpassDependency dependency { };
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // Take last operating 
            dependency.dstSubpass = 0; // ours
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Wait for finish reading from it from another process
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            
            CreateRenderAttachment(baseColorAttachmentDesc, &_colorAttachment);
            
            CreateRenderSubPass(VK_PIPELINE_BIND_POINT_GRAPHICS)
                .AddColorAttachment(_colorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                .AddDependency(dependency)
                .GetSubPassPtr(&_baseSubPass);

            _swapChainRecreatedListener = vulkanController->GetVulkanEventBus().CreateListener<Events::OnViewportResized>();
            _swapChainRecreatedListener->BindCallback(&MainRenderPass::SwapChainRecreatedHandler, this);
        }

        void PostInitialize() override
        {
            RecreateFrameBuffers(800, 600);
        }

        void OnTerminate() override
        {
            RenderPass::OnTerminate();
            
            for (const VkFramebuffer framebuffer : _framebuffers)
            {
                vkDestroyFramebuffer(GetVulkanInstanceController()->GetLogicalDevice(), framebuffer, nullptr);
            }

            for (const VkImageView imageView : _renderImagesViews)
            {
                vkDestroyImageView(GetVulkanInstanceController()->GetLogicalDevice(), imageView, nullptr);
            }

            for (const VkImage image : _renderImages)
            {
                vkDestroyImage(GetVulkanInstanceController()->GetLogicalDevice(), image, nullptr);
            }

            for (const VkDeviceMemory memory : _renderImageMem)
            {
                vkFreeMemory(GetVulkanInstanceController()->GetLogicalDevice(), memory, nullptr);
            }
        }

    public:
        Vulkan::PipelineLayout* CreateBaseSubPassPipelineLayout()
        {
            auto pipelineLayout = new Vulkan::PipelineLayout(this, _baseSubPass->ID);
            if (!InitializeSubController(pipelineLayout))
            {
                pipelineLayout->Terminate();
                return nullptr;
            }
            
            return pipelineLayout;
        }

        VkFramebuffer GetVkFramebuffer(uint32_t p_index) const
        {
            return _framebuffers[p_index];
        }

        VkImage GetVkImage(uint32_t p_index) const
        {
            return _renderImages[p_index];
        }

        VkImageView GetVkImageView(uint32_t p_index) const
        {
            return _renderImagesViews[p_index];
        }

        const std::vector<VkFramebuffer>& GetAllVkFramebuffer() const
        {
            return _framebuffers;
        }

    private:
        void RecreateFrameBuffers(uint32_t p_width, uint32_t p_height)
        {
            uint32_t imageCount = static_cast<uint32_t>(GetVulkanInstanceController()->GetSwapChainImageViews().size());
            glm::vec2 framebufferSize = GetVulkanInstanceController()->GetFrameBufferSize();

            for (const VkFramebuffer framebuffer : _framebuffers)
            {
                vkDestroyFramebuffer(GetVulkanInstanceController()->GetLogicalDevice(), framebuffer, nullptr);
            }

            for (const VkImageView imageView : _renderImagesViews)
            {
                vkDestroyImageView(GetVulkanInstanceController()->GetLogicalDevice(), imageView, nullptr);
            }

            for (const VkImage image : _renderImages)
            {
                vkDestroyImage(GetVulkanInstanceController()->GetLogicalDevice(), image, nullptr);
            }

            for (const VkDeviceMemory memory : _renderImageMem)
            {
                vkFreeMemory(GetVulkanInstanceController()->GetLogicalDevice(), memory, nullptr);
            }

            _renderImages.clear();
            _renderImages.resize(imageCount);
            _renderImagesViews.clear();
            _renderImagesViews.resize(imageCount);
            _framebuffers.clear();
            _framebuffers.resize(imageCount);
            _renderImageMem.clear();
            _renderImageMem.resize(imageCount);
            
            for (uint32_t i = 0; i < imageCount; i++)
            {
                CreateImage(p_width, p_height,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL | VK_IMAGE_LAYOUT_GENERAL,
                    _renderImages[i],
                    _renderImageMem[i]);

                VkImageViewCreateInfo viewInfo{};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image = _renderImages[i];
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = 1;

                VkResult result = vkCreateImageView(
                    GetVulkanInstanceController()->GetLogicalDevice(),
                    &viewInfo,
                    nullptr,
                    &_renderImagesViews[i]);

                if (result != VK_SUCCESS)
                {
                    continue;
                }
                
                VkImageView attachments[] { _renderImagesViews[i] };
                    
                VkFramebufferCreateInfo createInfo { };
                createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                createInfo.renderPass = GetVkRenderPass();
                createInfo.attachmentCount = 1;
                createInfo.pAttachments = attachments;
                createInfo.width = framebufferSize.x;
                createInfo.height = framebufferSize.y;
                createInfo.layers = 1;

                VkFramebuffer _framebuffer;
                result = vkCreateFramebuffer(
                    GetVulkanInstanceController()->GetLogicalDevice(),
                    &createInfo,
                    nullptr,
                    &_framebuffer);

                if (result != VK_SUCCESS)
                {
                    VULKAN_ERR("Failed to create SwapChain image view! (result: {})", string_VkResult(result));
                    continue;
                }

                _framebuffers[i] = _framebuffer;
            }

            GetVulkanInstanceController()->GetRendererEventBus().Publish<MainRenderPassRecreatedAttachment>();
        }

        void CreateImage(uint32_t p_width, uint32_t p_height, VkFormat p_format, VkImageTiling p_tiling,
            VkImageUsageFlags p_usage, VkMemoryPropertyFlags p_properties, VkImage& p_image, VkDeviceMemory& p_imageMemory)
        {
            VkImageCreateInfo imageInfo { };
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = p_width;
            imageInfo.extent.height = p_height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = p_format;
            imageInfo.tiling = p_tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = p_usage;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            auto result = vkCreateImage(GetVulkanInstanceController()->GetLogicalDevice(), &imageInfo, nullptr, &p_image);
            if (result != VK_SUCCESS)
            {
                return;
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(GetVulkanInstanceController()->GetLogicalDevice(), p_image, &memRequirements);

            VkMemoryAllocateInfo allocInfo { };
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, p_properties);

            result = vkAllocateMemory(GetVulkanInstanceController()->GetLogicalDevice(), &allocInfo, nullptr, &p_imageMemory);
            if (result != VK_SUCCESS)
            {
                return;
            }

            vkBindImageMemory(GetVulkanInstanceController()->GetLogicalDevice(), p_image, p_imageMemory, 0);
        }

        uint32_t FindMemoryType(uint32_t p_typeFilter, VkMemoryPropertyFlags p_properties)
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(GetVulkanInstanceController()->GetPhysicalDevice(), &memProperties);
            
            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
            {
                if ((p_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & p_properties) == p_properties)
                {
                    return i;
                }
            }

            // do sth ;-;
            return 0;
        }

        Architecture::EventResult SwapChainRecreatedHandler(const Events::OnViewportResized& p_event)
        {
            RecreateFrameBuffers(p_event.NewViewportSize.x, p_event.NewViewportSize.y);
            return Architecture::Internal::PASS;
        }

    private:
        std::shared_ptr<Architecture::EventListener<Events::OnViewportResized>> _swapChainRecreatedListener;
        
        RenderSubPass* _baseSubPass;
        const RenderAttachment* _colorAttachment;

        std::vector<VkImage> _renderImages;
        std::vector<VkDeviceMemory> _renderImageMem;
        std::vector<VkImageView> _renderImagesViews;
        std::vector<VkFramebuffer> _framebuffers;
    };
    
}
