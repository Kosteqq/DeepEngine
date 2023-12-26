#pragma once
#include "Architecture/EngineSystem.h"
#include "Vulkan/Instance/VulkanInstance.h"

#define MESSENGER_UTILS
#include "MainRenderPass.h"
#include "RendererCommandRecorder.h"
#include "Vulkan/Semaphore.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/ShaderModule.h"
#include "Vulkan/Debug/VulkanDebug.h"
#include "Vulkan/Fence.h"
#include "Vulkan/CommandBuffer.h"

namespace DeepEngine::Renderer
{
    struct FrameInFlight
    {
        Vulkan::CommandBuffer* _commandBuffer;
        Vulkan::Fence* _fence;
        
    };
    
    class RendererSubsystem final : Architecture::EngineSubsystem
    {
    public:
        RendererSubsystem(Architecture::EventBus& p_engineEventBus)
            : EngineSubsystem(p_engineEventBus, "Renderer")
        { 
            _vulkanInstance = new Vulkan::VulkanInstance(p_engineEventBus);
            _wndChangeMinimizedListener = _internalSubsystemEventBus.CreateListener<Events::OnWindowChangeMinimized>();
            _wndChangeMinimizedListener->BindCallback(&RendererSubsystem::WindowChangedMinimizedHandler, this);
        }

    protected:
        bool Init() override
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
            auto vertShader = new Vulkan::ShaderModule("../DeepEngine/Renderer/Shader/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
            if (!_vulkanInstance->InitializeSubController(vertShader))
            {
                return false;
            }
            
            auto fragShader = new Vulkan::ShaderModule("../DeepEngine/Renderer/Shader/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
            if (!_vulkanInstance->InitializeSubController(fragShader))
            {
                return false;
            }

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
            
            _trianglePipeline = new Vulkan::GraphicsPipeline(pipelineLayout, vertShader, fragShader, dynamicState,
                colorBlend, { attachmentBlend }, rasterization);

            if (!pipelineLayout->InitializeSubController(_trianglePipeline))
            {
                return false;
            }

            _commandRecorder = RendererCommandRecorder(_vulkanInstance, _mainGraphicsQueue);

            return true;
        }
        
        void Destroy() override
        {
            _commandRecorder.Terminate();
            _vulkanInstance->Terminate();
            Vulkan::VulkanDebugger::Terminate();
        }
        
        void Tick() override
        {
            if (_isWindowMinimized)
            {
                return;
            }

            vkWaitForFences(
                _vulkanInstance->GetLogicalDevice(),
                1,
                _readyToRenderFence->GetVkFencePtr(),
                VK_TRUE,
                UINT64_MAX);

            uint32_t imageIndex;
            const auto acquireResult = vkAcquireNextImageKHR(
                _vulkanInstance->GetLogicalDevice(),
                _vulkanInstance->GetSwapchain(),
                UINT64_MAX,
                _availableImageToRenderSemaphore->GetVkSemaphore(),
                VK_NULL_HANDLE,
                &imageIndex);

            bool invalidSwapChain = false;
            if (acquireResult == VK_SUBOPTIMAL_KHR)
            {
                invalidSwapChain = true;
            }
            if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
            {
                _vulkanInstance->RecreateSwapChain();
                return;
            }
            
            vkResetFences(_vulkanInstance->GetLogicalDevice(), 1, _readyToRenderFence->GetVkFencePtr());

            _commandRecorder.RecordBuffer({0.05f, 0.05f, 0.15f, 1.0f},
                imageIndex, _mainRenderPass, _trianglePipeline);

            _commandRecorder.SubmitBuffer(_readyToRenderFence,
                { _availableImageToRenderSemaphore },
                { _finishRenderingSemaphore });

            VkSwapchainKHR swapChains[] = { _vulkanInstance->GetSwapchain() };
            VkSemaphore waitSemaphores[] = { _finishRenderingSemaphore->GetVkSemaphore() };
            
            VkPresentInfoKHR presentInfo { };
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = waitSemaphores;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr;

            VkResult presentResult = vkQueuePresentKHR(_mainGraphicsQueue->Queue, &presentInfo);
            if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || invalidSwapChain)
            {
                _vulkanInstance->RecreateSwapChain();
            }
            else if (presentResult != VK_SUCCESS)
            {
                VULKAN_ERR("Failed to present swapchain with returned result {}", string_VkResult(presentResult));
            }
        }

    private:
        bool InitializeVulkanInstance();
        bool EnableGlfwExtensions();
        Architecture::EventResult WindowChangedMinimizedHandler(const Events::OnWindowChangeMinimized& p_event);

    private:
        Vulkan::VulkanInstance* _vulkanInstance = nullptr;
        MainRenderPass* _mainRenderPass = nullptr;
        Vulkan::GraphicsPipeline* _trianglePipeline = nullptr;
        Vulkan::Fence* _readyToRenderFence = nullptr;
        Vulkan::Semaphore* _availableImageToRenderSemaphore = nullptr; 
        Vulkan::Semaphore* _finishRenderingSemaphore = nullptr; 
 
        const Vulkan::VulkanInstance::QueueInstance* _mainGraphicsQueue = nullptr;

        RendererCommandRecorder _commandRecorder;

        bool _isWindowMinimized = false;

        std::shared_ptr<Architecture::EventListener<Events::OnWindowChangeMinimized>> _wndChangeMinimizedListener;
    };
    
}
