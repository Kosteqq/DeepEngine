#include "VulkanPipelineLayout.h"

namespace DeepEngine::Renderer::Vulkan
{
    VulkanPipelineLayout::VulkanPipelineLayout(VulkanRenderPass* p_renderPass, uint32_t p_subPassIndex)
        : _renderPass(p_renderPass), _subPassIndex(p_subPassIndex)
    { }

    bool VulkanPipelineLayout::OnInitialize()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo { };
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        VULKAN_CHECK_CREATE(
            vkCreatePipelineLayout(
                GetVulkanInstanceController()->GetLogicalDevice(),
                &pipelineLayoutCreateInfo,
                nullptr,
                &_pipelineLayout),
            "Failed to create Vulkan pipeline layout!")

        return true;
    }

    void VulkanPipelineLayout::OnTerminate()
    {
        vkDestroyPipelineLayout(GetVulkanInstanceController()->GetLogicalDevice(), _pipelineLayout, nullptr);
    }
}
