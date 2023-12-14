#pragma once
#include <unordered_map>

#include "Debugs/Logger.h"
#include "VulkanPhysicalLayer.h"
#include "VulkanDebug.h"


namespace DeepEngine::Renderer
{
    class VulkanLogicalLayer
    {
    public:
        VulkanLogicalLayer(std::shared_ptr<Debug::Logger> p_logger, const VulkanPhysicalLayer* p_physicalLayer,
            const VkSurfaceKHR& p_surface);
        ~VulkanLogicalLayer();

        bool Init(const VulkanDebug* p_vulkanDebug);
        void Terminate();

        const VkDevice& GetLogicalDevice() const
        { return _logicalDevice; }
        VkQueue& GetGraphicsQueue(const uint32_t p_id)
        { return _queueMap[p_id]; }
        const std::vector<uint32_t>& GetQueuesFamilies() const
        { return _createQueueIds; }

        bool AddQueue(uint32_t p_id, VkQueueFlagBits p_queueFeatures, bool p_requireSurfaceSupport);
        
    private:
        bool _initialized = false;
        
        const VulkanInstance* _instance = nullptr;
        const VulkanPhysicalLayer* _physicalLayer = nullptr;
        const VkSurfaceKHR& _surface;
        float _queuePriority = 1;

        VkDevice _logicalDevice;
        std::unordered_map<uint32_t, VkQueue> _queueMap;
        std::vector<VkQueueFamilyProperties> _availableQueueFamilies;

        std::vector<VkDeviceQueueCreateInfo> _createQueuesInfo;
        std::vector<uint32_t> _createQueueIds;
        
        std::shared_ptr<Debug::Logger> _logger;
    };
}
