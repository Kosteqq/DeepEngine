#include "BaseVulkanController.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
    VulkanInstance* BaseVulkanController::_vulkanInstance = nullptr;

    void BaseVulkanController::Terminate()
    {
        if (_parentController != nullptr)
        {
            _parentController->_childControllers.erase(this);
        }

        for (auto child : _childControllers)
        {
            child->_parentController = nullptr;
            child->Terminate();
        }

        _childControllers.clear();

        OnTerminate();
        
        delete this;
    }
}
