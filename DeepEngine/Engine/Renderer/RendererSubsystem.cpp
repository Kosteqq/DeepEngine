#include "RendererSubsystem.h"

namespace DeepEngine::Engine::Renderer
{
    RendererSubsystem::RendererSubsystem(Core::Events::EventBus& p_engineEventBus): EngineSubsystem(p_engineEventBus, "Renderer")
    { 
        _vulkanInstance = new Vulkan::VulkanInstance(p_engineEventBus, _internalSubsystemEventBus);
        _wndChangeMinimizedListener = _internalSubsystemEventBus.CreateListener<Core::Events::OnWindowChangeMinimized>();
        _wndChangeMinimizedListener->BindCallback(&RendererSubsystem::WindowChangedMinimizedHandler, this);

        _factory = std::make_unique<Vulkan::Factory>(*_vulkanInstance);
        _factory->Bind();
    }

    bool RendererSubsystem::Init()
    {
        if (!InitializeVulkanInstance())
        {
            return false;
        }

        _readyToRenderFence = Vulkan::Factory::SubFactory<Vulkan::Fence>::CreateSignaled();

        _availableImageToRenderSemaphore = Vulkan::Factory::SubFactory<Vulkan::Semaphore>::Create();

        _finishRenderingSemaphore = Vulkan::Factory::SubFactory<Vulkan::Semaphore>::Create();
        
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
        if(!_renderers[0].Init("../DeepEngine/Engine/Renderer/Shader/vert.spv",
            "../DeepEngine/Engine/Renderer/Shader/frag.spv",
                            dynamicState, colorBlend, attachmentBlend, rasterization, pipelineLayout))
        {
            return false;
        }
        if(!_renderers[1].Init("../DeepEngine/Engine/Renderer/Shader/vert1.spv",
            "../DeepEngine/Engine/Renderer/Shader/frag1.spv",
                            dynamicState, colorBlend, attachmentBlend, rasterization, pipelineLayout))
        {
            return false;
        }
        
        _commandRecorder = RendererCommandRecorder(_vulkanInstance, _mainGraphicsQueue);
        
        _imGuiController = new ImGuiController(_vulkanInstance, _mainGraphicsQueue, _mainRenderPass);

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
    
    Core::Events::EventResult RendererSubsystem::WindowChangedMinimizedHandler(
        const Core::Events::OnWindowChangeMinimized& p_event)
    {
        _isWindowMinimized = p_event.MinimizedMode;
        return Core::Events::EventResult::PASS;
    }
}
