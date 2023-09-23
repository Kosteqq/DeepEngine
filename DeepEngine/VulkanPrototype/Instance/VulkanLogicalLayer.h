#pragma once
#include "Debugs/Logger.h"
#include "VulkanPhysicalLayer.h"
#include "VulkanDebug.h"


namespace DeepEngine::Renderer
{
    class VulkanLogicalLayer
    {
    public:
        VulkanLogicalLayer(std::shared_ptr<Core::Debug::Logger> p_logger, const VulkanPhysicalLayer* p_physicalLayer);
        ~VulkanLogicalLayer();

        bool Init(const VulkanDebug* p_vulkanDebug);
        void Terminate();

        const VkDevice& GetLogicalDevice() const
        { return _logicalDevice; }
        const VkQueue& GetGraphicsQueue() const
        { return _graphicsQueue; }

        void AddQueue(VkQueueFlagBits p_queueFeatures);
        
    private:
        bool _initialized = false;
        
        const VulkanInstance* _instance = nullptr;
        const VulkanPhysicalLayer* _physicalLayer = nullptr;
        
        VkDevice _logicalDevice;
        VkQueue _graphicsQueue;

        std::vector<VkDeviceQueueCreateInfo> _createQueuesInfo { };
        std::vector<VkQueueFamilyProperties> _availableQueueFamilies;

        std::shared_ptr<Core::Debug::Logger> _logger;
    };
}
