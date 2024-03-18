#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class Semaphore final : public BaseVulkanController
    {
    public:
        Semaphore() = default;
        ~Semaphore() override = default;

        VkSemaphore GetVkSemaphore() const
        { return _semaphore; }

        const VkSemaphore* GetVkSemaphorePtr() const
        { return &_semaphore; }

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

    class Semaphore2 : public VulkanObject
    {
    public:
        Semaphore2(VkSemaphore p_handler) : _handler(p_handler)
        { }
        
        VkSemaphore GetHandler() const
        { return _handler; }
        
        const VkSemaphore _handler;
    };
    
    template<>
    class VulkanFactory::SubFactory<Semaphore2>
    {
    public:
        template <VulkanObjectKind TParent>
        static VulkanRef<Semaphore2> Create(const VulkanRef<TParent>& p_parent)
        {
            VkSemaphoreCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            createInfo.flags = 0;

            VkSemaphore handler;
            vkCreateSemaphore(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &createInfo,
                nullptr,
                &handler);

            return CreateObject(new Semaphore2(handler), Terminate, p_parent);
        }
        
        static VulkanRef<Semaphore2> Create()
        {
            VkSemaphoreCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            createInfo.flags = 0;
        
            VkSemaphore handler;
            vkCreateSemaphore(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &createInfo,
                nullptr,
                &handler);
        
            return CreateObject<Semaphore2, VulkanObject>(new Semaphore2(handler), Terminate);
        }

        static void Terminate(VulkanObject* p_object)
        {
            vkDestroySemaphore(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                ((Semaphore2*)p_object)->GetHandler(),
                nullptr);
        }
    };
    
}
