#pragma once
#include "Vulkan/RenderPass.h"
#include "Vulkan/PipelineLayout.h"
#include "Vulkan/Events/VulkanEvents.h"
#include "Events.h"

namespace DeepEngine::Engine::Renderer
{

    BEGIN_LOCAL_EVENT_DEFINITION(MainRenderPassRecreatedAttachment)
    END_EVENT_DEFINITION

    class MainRenderPassBuilder : public Vulkan::RenderPassBuilder
    {
    public:
        MainRenderPassBuilder(const Vulkan::VulkanInstance* p_vulkanInstance)
            : _vulkanInstance(p_vulkanInstance)
        { }

        Vulkan::RenderSubPassHandler GetMainSubPass() const
        { return _mainSubPass; }
        
    protected:
        void DeclareAttachments() override
        {
            VkAttachmentDescription colorAttachmentDesc { };
            colorAttachmentDesc.format = VK_FORMAT_R8G8B8A8_SRGB;
            colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            _colorAttachment = CreateAttachment(colorAttachmentDesc);
            
            _mainSubPass = CreateRenderSubPass(VK_PIPELINE_BIND_POINT_GRAPHICS);
            AttachColorToSubPass(_mainSubPass, _colorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            AttachDependencyToSubPass(
                nullptr,
                &_mainSubPass,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        }
        
    private:
        const Vulkan::VulkanInstance* _vulkanInstance;
        Vulkan::RenderAttachmentHandler _colorAttachment;
        Vulkan::RenderSubPassHandler _mainSubPass;
    };

    class MainRenderPassController
    {
    public:
        MainRenderPassController(Vulkan::VulkanInstance* p_vulkanInstance)
            : _vulkanInstance(p_vulkanInstance)
        {
            MainRenderPassBuilder builder(_vulkanInstance);
            _renderPass = Vulkan::Factory::SubFactory<Vulkan::RenderPass2>::Create(&builder);
            _mainPipelineLayout = Vulkan::Factory::SubFactory<Vulkan::PipelineLayout2>::Create(_renderPass, _mainSubPass);
            
            RecreateFrameBuffers(800, 600);
        }

        ~MainRenderPassController()
        {
            DestroyVulkanStuff();
        }

        VkFramebuffer GetFrameBufferHandler(uint32_t p_index) const
        { return _framebuffers[p_index]; }

        VkImage GetFrameImageHandler(uint32_t p_index) const
        { return _renderImages[p_index]; }

        VkImageView GetFrameImageViewHandler(uint32_t p_index) const
        { return _renderImagesViews[p_index]; }

        uint64_t GetFrameBuffersCount() const
        { return _framebuffers.size(); }

        VkRenderPass GetRenderPassHandler() const
        { return _renderPass->GetHandler(); }

        Vulkan::Ref<Vulkan::RenderPass2> GetRenderPass() const
        { return _renderPass; }

        Vulkan::Ref<Vulkan::PipelineLayout2> GetMainSubPassPipelineLayout() const
        { return _mainPipelineLayout; }

    private:
        void DestroyVulkanStuff()
        {
            for (const VkFramebuffer framebuffer : _framebuffers)
            {
                vkDestroyFramebuffer(_vulkanInstance->GetLogicalDevice(), framebuffer, nullptr);
            }

            for (const VkImageView imageView : _renderImagesViews)
            {
                vkDestroyImageView(_vulkanInstance->GetLogicalDevice(), imageView, nullptr);
            }

            for (const VkImage image : _renderImages)
            {
                vkDestroyImage(_vulkanInstance->GetLogicalDevice(), image, nullptr);
            }

            for (const VkDeviceMemory memory : _renderImageMem)
            {
                vkFreeMemory(_vulkanInstance->GetLogicalDevice(), memory, nullptr);
            }

            _framebuffers.clear();
            _renderImagesViews.clear();
            _renderImages.clear();
            _renderImageMem.clear();
        }
        
        void RecreateFrameBuffers(uint32_t p_width, uint32_t p_height)
        {
            uint32_t imageCount = static_cast<uint32_t>(_vulkanInstance->GetSwapChainImageViews().size());
            glm::vec2 framebufferSize = _vulkanInstance->GetFrameBufferSize();

            DestroyVulkanStuff();

            _renderImages.resize(imageCount);
            _renderImagesViews.resize(imageCount);
            _framebuffers.resize(imageCount);
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
                    _vulkanInstance->GetLogicalDevice(),
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
                createInfo.renderPass = _renderPass->GetHandler();
                createInfo.attachmentCount = 1;
                createInfo.pAttachments = attachments;
                createInfo.width = framebufferSize.x;
                createInfo.height = framebufferSize.y;
                createInfo.layers = 1;

                VkFramebuffer _framebuffer;
                result = vkCreateFramebuffer(
                    _vulkanInstance->GetLogicalDevice(),
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

            _vulkanInstance->GetRendererEventBus().Publish<MainRenderPassRecreatedAttachment>();
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

            auto result = vkCreateImage(_vulkanInstance->GetLogicalDevice(), &imageInfo, nullptr, &p_image);
            if (result != VK_SUCCESS)
            {
                return;
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(_vulkanInstance->GetLogicalDevice(), p_image, &memRequirements);

            VkMemoryAllocateInfo allocInfo { };
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, p_properties);

            result = vkAllocateMemory(_vulkanInstance->GetLogicalDevice(), &allocInfo, nullptr, &p_imageMemory);
            if (result != VK_SUCCESS)
            {
                return;
            }

            vkBindImageMemory(_vulkanInstance->GetLogicalDevice(), p_image, p_imageMemory, 0);
        }

        uint32_t FindMemoryType(uint32_t p_typeFilter, VkMemoryPropertyFlags p_properties) const
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(_vulkanInstance->GetPhysicalDevice(), &memProperties);
            
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

        Core::Events::EventResult SwapChainRecreatedHandler(const Events::OnViewportResized& p_event)
        {
            RecreateFrameBuffers(p_event.NewViewportSize.x, p_event.NewViewportSize.y);
            return Core::Events::EventResult::PASS;
        }

    private:
        const Vulkan::VulkanInstance* _vulkanInstance;
        
        std::shared_ptr<Core::Events::EventListener<Events::OnViewportResized>> _swapChainRecreatedListener;

        Vulkan::Ref<Vulkan::RenderPass2> _renderPass;
        Vulkan::RenderSubPassHandler _mainSubPass;
        
        Vulkan::Ref<Vulkan::PipelineLayout2> _mainPipelineLayout;

        std::vector<VkImage> _renderImages;
        std::vector<VkDeviceMemory> _renderImageMem;
        std::vector<VkImageView> _renderImagesViews;
        std::vector<VkFramebuffer> _framebuffers;
    };
}
