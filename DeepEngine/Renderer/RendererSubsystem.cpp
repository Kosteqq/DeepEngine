#include "RendererSubsystem.h"

namespace DeepEngine::Renderer
{
    bool RendererSubsystem::Init()
    {
        if (!InitializeVulkanInstance())
        {
            return false;
        }

        _readyToRenderFence = new Vulkan::Fence(true);
        if (!_vulkanInstance->InitializeSubController(_readyToRenderFence))
        {
            return false;
        }

        _availableImageToRenderSemaphore = new Vulkan::Semaphore();
        if (!_vulkanInstance->InitializeSubController(_availableImageToRenderSemaphore))
        {
            return false;
        }

        _finishRenderingSemaphore = new Vulkan::Semaphore();
        if (!_vulkanInstance->InitializeSubController(_finishRenderingSemaphore))
        {
            return false;
        }

        _mainRenderPass = new MainRenderPass();
        if (!_vulkanInstance->InitializeSubController(_mainRenderPass))
        {
            return false;
        }

        Vulkan::PipelineLayout* pipelineLayout = _mainRenderPass->CreateBaseSubPassPipelineLayout();

        Vulkan::PipelineDynamicState dynamicState {
            .Viewport = true,
            .Scissor = true,
            .LineWidth = false,
        };

        Vulkan::PipelineColorBlend colorBlend {
            .ColorBlendConstants = { 1.0f, 1.0f, 1.0f, 1.0f },
            .EnableLogicalBlendOperation = false,
        };

        Vulkan::PipelineColorBlendAttachment attachmentBlend {
            .WriteChannelR = true,
            .WriteChannelG = true,
            .WriteChannelB = true,
            .WriteChannelA = true,
            .EnableBlend = false,
        };

        Vulkan::PipelineRasterization rasterization {
            .EnableDepthClamp = false,
            .EnableDiscardRasterizer = false,
            .EnableDepthBias = false,
            .PolygonMode = VK_POLYGON_MODE_FILL,
            .CullMode = VK_CULL_MODE_NONE,
            .FrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        };

        _renderers.resize(2);
        if(!_renderers[0].Init("../DeepEngine/Renderer/Shader/RedTriangle_Vert.spv",
            "../DeepEngine/Renderer/Shader/RedTriangle_Frag.spv",
                            dynamicState, colorBlend, attachmentBlend, rasterization, pipelineLayout))
        {
            return false;
        }
        if(!_renderers[1].Init("../DeepEngine/Renderer/Shader/vert1.spv",
            "../DeepEngine/Renderer/Shader/frag1.spv",
                            dynamicState, colorBlend, attachmentBlend, rasterization, pipelineLayout))
        {
            return false;
        }
        
        _commandRecorder = RendererCommandRecorder(_vulkanInstance, _mainGraphicsQueue);

        return true;
    }

    bool RendererSubsystem::InitializeVulkanInstance()
    {
        MESSENGER_PREINITIALIZE(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
            
        Vulkan::VulkanDebugger::TryAddValidationLayer("VK_LAYER_KHRONOS_validation");

        if (_vulkanInstance->IsInstanceExtensionAvailable(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            _vulkanInstance->EnableInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        if (!EnableGlfwExtensions())
        {
            return false;
        }
            
        if (!_vulkanInstance->InitializeInstance())
        {
            return false;
        }

        _vulkanInstance->EnablePhysicalExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        if (!_vulkanInstance->InitializePhysicalDevice())
        {
            return false;
        }

        if (!_vulkanInstance->TryAddQueueToCreate(VK_QUEUE_GRAPHICS_BIT, true, &_mainGraphicsQueue))
        {
            return false;
        }

        if (!_vulkanInstance->InitializeLogicalDevice())
        {
            return false;
        }

        const auto& availableFormats = _vulkanInstance->GetAvailableSurfaceFormats();
        VkSurfaceFormatKHR bestFormat = availableFormats[0];
        for (int i = 0; i < (uint32_t)availableFormats.size(); i++)
        {
            if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB
                && availableFormats[i].colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)
            {
                bestFormat = availableFormats[i];
                break;
            }
        }

        _vulkanInstance->SetSwapChainFormat(bestFormat, VK_PRESENT_MODE_MAILBOX_KHR, true);

        if (!_vulkanInstance->InitializeSwapChain())
        {
            return false;
        }

        return true;
    }

    bool RendererSubsystem::EnableGlfwExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (int i = 0; i < glfwExtensionCount; i++)
        {
            if (!_vulkanInstance->IsInstanceExtensionAvailable(glfwExtensions[i]))
            {
                return false;
            }

            _vulkanInstance->EnableInstanceExtension(glfwExtensions[i]);
        }

        return true;
    }
}
