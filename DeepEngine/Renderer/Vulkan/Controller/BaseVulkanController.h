#pragma once
#include <list>
#include <type_traits>
#include <iostream>
#include <unordered_set>

namespace DeepEngine::Renderer::Vulkan
{
    class VulkanInstance;
    
    class BaseVulkanController
    {
        friend class VulkanInstance;

    protected:
        BaseVulkanController() = default;
        virtual ~BaseVulkanController() = default;

    public:
        BaseVulkanController(BaseVulkanController& p_origin) = delete;
        BaseVulkanController(const BaseVulkanController&& p_origin) = delete;

        BaseVulkanController& operator=(const BaseVulkanController& p_other) = delete;
            
        void Terminate();

        template <typename T>
        requires std::is_base_of_v<BaseVulkanController, T>
        bool InitializeSubController(T* p_controller)
        {
            auto baseController = (BaseVulkanController*)p_controller;
            baseController->_parentController = this;
            _childControllers.insert((BaseVulkanController*)p_controller);

            baseController->_debugTypeName = typeid(T).name();
            return baseController->OnInitialize();
        }

        const char* GetDebugTypeName() const
        { return _debugTypeName; }
        
        const std::unordered_set<BaseVulkanController*>& GetChildControllers() const
        { return _childControllers; }

    protected:
        virtual bool OnInitialize() = 0;
        virtual void OnTerminate() = 0;

        VulkanInstance* GetVulkanInstanceController() const
        { return _vulkanInstance; }

    private:
        static VulkanInstance* _vulkanInstance;
        
        std::unordered_set<BaseVulkanController*> _childControllers;
        BaseVulkanController* _parentController = nullptr;
        
        const char* _debugTypeName;
    };
}
