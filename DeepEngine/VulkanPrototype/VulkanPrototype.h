#pragma once
#include <fstream>

#define NOMINMAX
#include <vulkan/vulkan.h>
#include <fmt/format.h>

#include "Debugs/Logger.h"
#include "Architecture/EngineSystem.h"
#include "Instance/VulkanCommandPool.h"
#include "Instance/VulkanDebug.h"
#include "Instance/VulkanInstance.h"
#include "Instance/VulkanLogicalLayer.h"
#include "Instance/VulkanPhysicalLayer.h"
#include "Instance/VulkanRenderPass.h"
#include "Instance\VulkanPipelineLayout.h"
#include "Instance/VulkanSwapChain.h"
#include "Window/WindowSubsystem.hpp"

namespace DeepEngine::Renderer
{
    class VulkanPrototype : public Architecture::EngineSubsystem, Architecture::EventListener<Events::OnCreateGlfwContext>
    {
    public:
        VulkanPrototype() : EngineSubsystem("Vulkan Renderer Prot")
        {
            _vulkanLogger = Debug::Logger::CreateLoggerInstance("VULKAN");
        }

        ~VulkanPrototype()
        {
            _commandPool->Terminate();
            
            for (auto framebuffer : _swapChainFramebuffers)
                {
                    vkDestroyFramebuffer(_logicalLayer->GetLogicalDevice(), framebuffer, nullptr);
            }
            
            _pipeline->Terminate();
            _renderPass->Terminate();
            _pipeline->Terminate();
            _swapChain->Terminate();
            // Queue also will be destroy
            _logicalLayer->Terminate();
            // Physical device will be destroy anyway

            vkDestroySurfaceKHR(_vulkanInstance->GetInstance(), _surface, nullptr);
            _vulkanInstance->Terminate();
            _vulkanDebug->Terminate();

            delete _commandPool;
            delete _pipeline;
            delete _renderPass;
            delete _swapChain;
            delete _logicalLayer;
            delete _physicalLayer;
            delete _vulkanInstance;
            delete _vulkanDebug;
        }

    protected:
        bool EventHandler(const Events::OnCreateGlfwContext* p_event) override
        {
            _glfwWindow = p_event->GLFWWindow;
            return false;
        }

    protected:
        bool Init() override
        {
            const auto messageTypes = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            
            // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            const auto messageLevels = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                
            _vulkanDebug = new VulkanDebug(_vulkanLogger, messageLevels, messageTypes);

            if (!_vulkanDebug->TryAddLayer("VK_LAYER_KHRONOS_validation"))
            {
                WARN("Failed to enable base vulkan validation layer");
            }
            
            _vulkanInstance = new VulkanInstance(_vulkanLogger);
            
            if (!EnableExtensions())
            {
                ERR("Failed to enable at least one of extensions!");
                return false;
            }

            if (!_vulkanInstance->Init(_vulkanDebug))
            {
                FAIL_MILESTONE(InitializeVulkanDebug);
            }
            else
            {
                FULFIL_MILESTONE(InitializeVulkanDebug);
            }
            
            if (!_vulkanDebug->Init(&_vulkanInstance->GetInstance()))
            {
                FAIL_MILESTONE(CreateVulkanInstance);
                return false;
            }
            FULFIL_MILESTONE(CreateVulkanInstance);

            // DIRTY
            if (_glfwWindow == nullptr)
            {
                ERR("GLFW context is null but should be already created!");
            }
            
            if (glfwCreateWindowSurface(_vulkanInstance->GetInstance(), _glfwWindow, nullptr, &_surface))
            {
                return false;
            }
            
            
            // \DIRTY
            

            _physicalLayer = new VulkanPhysicalLayer(_vulkanLogger, _vulkanInstance);

            if (!_physicalLayer->Init())
            {
                FAIL_MILESTONE(InitializeVulkanPhysicalDevice);
                return false;
            }
            FULFIL_MILESTONE(InitializeVulkanPhysicalDevice);
            
            if (!_physicalLayer->TryEnableExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
            {
                return false;
            }

            _logicalLayer = new VulkanLogicalLayer(_vulkanLogger, _physicalLayer, _surface);
            // For now only single queue is supported!
            _logicalLayer->AddQueue(0, VK_QUEUE_GRAPHICS_BIT, true);

            if (!_logicalLayer->Init(_vulkanDebug))
            {
                FAIL_MILESTONE(InitializeVulkanLogicalDevice);
                return false;
            }
            FULFIL_MILESTONE(InitializeVulkanLogicalDevice);

            uint32_t width = 800;
            uint32_t height = 600;
            _swapChain = new VulkanSwapChain(_vulkanLogger, _physicalLayer, _logicalLayer, _surface, width, height);
            if (!_swapChain->Init())
            {
                return false;
            }

            _renderPass = new VulkanRenderPass(_vulkanLogger, _logicalLayer, _swapChain);

            if (!_renderPass->Initialize())
            {
                return false;
            }

            _pipeline = new VulkanPipeline(_vulkanLogger, _logicalLayer, _swapChain, _renderPass);

            if (!_pipeline->Initialize())
            {
                return false;
            }

            CreateFramebuffers();

            _commandPool = new VulkanCommandPool(_vulkanLogger, _logicalLayer);
            if (!_commandPool->Init())
            {
                return false;
            }

            return true;
        }
        
        void Tick() override
        {
            const auto fence = _commandPool->GetInFlightFence();
            
            vkWaitForFences(_logicalLayer->GetLogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
            vkResetFences(_logicalLayer->GetLogicalDevice(), 1, &fence);

            uint32_t imageIndex;
            vkAcquireNextImageKHR(
                _logicalLayer->GetLogicalDevice(),
                _swapChain->GetSwapChain(),
                UINT64_MAX,
                _commandPool->GetImageAvailableSemaphore(),
                VK_NULL_HANDLE,
                &imageIndex);


            _commandPool->RecordCommandBuffer(_swapChainFramebuffers[imageIndex], _renderPass, _swapChain, _pipeline);

            // those two correlates with each other 
            VkSemaphore waitSemaphores[] = { _commandPool->GetImageAvailableSemaphore() };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            
            const auto commandBuff = _commandPool->GetCommandBuffer();
            
            VkSemaphore finishSemaphores[] = { _commandPool->GetRenderFinishedSemaphore() };
            
            VkSubmitInfo submitInfo { };
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuff;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = finishSemaphores;

            auto result = vkQueueSubmit(_logicalLayer->GetGraphicsQueue(0), 1, &submitInfo, fence);
            if (result != VK_SUCCESS)
            {
                LOG_ERR(_vulkanLogger, "Failed to submit draw command with returned result {}", string_VkResult(result));
            }

            VkSwapchainKHR swapChains[] = { _swapChain->GetSwapChain() };
            
            VkPresentInfoKHR presentInfo { };
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = finishSemaphores;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr;

            result = vkQueuePresentKHR(_logicalLayer->GetGraphicsQueue(0), &presentInfo);
            if (result != VK_SUCCESS)
            {
                LOG_ERR(_vulkanLogger, "Failed to present swapchain with returned result {}", string_VkResult(result));
            }
            
        }

        void Destroy() override
        {
        }
        
    private:
        bool EnableExtensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions= glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            for (int i = 0; i < glfwExtensionCount; i++)
            {
                if (!_vulkanInstance->TryEnableExtension(glfwExtensions[i]))
                {
                    return false;
                }
            }
            
            if (!_vulkanInstance->TryEnableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            {
                return false;
            }

            return true;
        }

        bool CreateFramebuffers()
        {
            const auto imageViews =  _swapChain->GetSwapChainImageViews();
            _swapChainFramebuffers.resize(imageViews.size());
            for (uint32_t i = 0; i < imageViews.size(); i++)
            {
                VkImageView attachments[] { imageViews[i] };
                
                VkFramebufferCreateInfo framebufferCreateInfo { };
                framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferCreateInfo.renderPass = _renderPass->GetVulkanRenderPass();
                framebufferCreateInfo.attachmentCount = 1;
                framebufferCreateInfo.pAttachments = attachments;
                framebufferCreateInfo.width = _swapChain->GetExtent().width;
                framebufferCreateInfo.height = _swapChain->GetExtent().height;
                framebufferCreateInfo.layers = 1;

                const auto result = vkCreateFramebuffer(_logicalLayer->GetLogicalDevice(), &framebufferCreateInfo,
                    nullptr, &_swapChainFramebuffers[i]);

                if (result != VK_SUCCESS)
                {
                    LOG_ERR(_vulkanLogger, "Failed to create framebuffer\nResult: {}", string_VkResult(result));
                    return false;
                }
            }
            
            return true;
        }

    private:
        VkSurfaceKHR _surface;
        GLFWwindow* _glfwWindow;

    private:
        VulkanDebug* _vulkanDebug;
        VulkanInstance* _vulkanInstance;
        VulkanPhysicalLayer* _physicalLayer;
        VulkanLogicalLayer* _logicalLayer;
        VulkanSwapChain* _swapChain;
        VulkanRenderPass* _renderPass;
        VulkanPipeline* _pipeline;
        VulkanCommandPool* _commandPool;
        
        std::vector<VkFramebuffer> _swapChainFramebuffers;
 
        std::shared_ptr<Debug::Logger> _vulkanLogger;

    private:
        DEFINE_MILESTONE(CreateVulkanInstance);
        DEFINE_MILESTONE(InitializeVulkanDebug);
        DEFINE_MILESTONE(InitializeVulkanPhysicalDevice);
        DEFINE_MILESTONE(InitializeVulkanLogicalDevice);
    };
    
}
