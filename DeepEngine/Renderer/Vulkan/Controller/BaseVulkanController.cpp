#include "BaseVulkanController.h"

namespace DeepEngine::Renderer::Vulkan
{
    VulkanInstance* BaseVulkanController::_vulkanInstance = nullptr;

    void BaseVulkanController::Terminate()
    {
        if (_parentController != nullptr)
        {
            _parentController->_dependentControllers.remove(this);
        }
        
        while (!_dependentControllers.empty())
        {
            BaseVulkanController* dependent = _dependentControllers.back();
            dependent->_parentController = nullptr;
            dependent->Terminate();
            
            _dependentControllers.remove(dependent);
        }

        OnTerminate();
        
        delete this;
    }
}
