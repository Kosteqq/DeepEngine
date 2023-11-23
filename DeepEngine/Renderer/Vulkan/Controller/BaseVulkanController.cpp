#include "BaseVulkanController.h"

namespace DeepEngine::Renderer::Vulkan
{
    BaseVulkanController::BaseVulkanController(VulkanInstance* p_vulkanInstance): _vulkanInstance(p_vulkanInstance)
    { }

    BaseVulkanController::~BaseVulkanController()
    {
        if (_subInstances.size() > 0)
        {
            // Delete all sub-instances
            for (auto subInstance : _subInstances)
            {
                if (subInstance != nullptr)
                {
                    subInstance->OnTerminate();
                    delete subInstance;
                }
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
