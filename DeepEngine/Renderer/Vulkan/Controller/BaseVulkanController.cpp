#include "BaseVulkanController.h"

namespace DeepEngine::Renderer::Vulkan
{
    VulkanInstance* BaseVulkanController::_vulkanInstance = nullptr;
    
    BaseVulkanController::~BaseVulkanController()
    {
        // Delete all sub-instances
        while (static_cast<uint32_t>(_subInstances.size()) > 0)
        {
            auto* subInstancePtr = _subInstances.top();
            _subInstances.pop();
            
            if (subInstancePtr != nullptr)
            {
                subInstancePtr->OnTerminate();
                delete subInstancePtr;
            }
        }
        
        auto** ptr = _pointerInContainer;

        if (ptr != nullptr)
        {
            // null pointer inside base-instance
            *ptr = nullptr;
        }
    }
}
