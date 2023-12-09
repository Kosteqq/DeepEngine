#pragma once
#include <list>

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
            ((BaseVulkanController*)p_controller)->_parentController = this;
            _dependentControllers.push_back((BaseVulkanController*)p_controller);

            return ((BaseVulkanController*)p_controller)->OnInitialize();
        }

    protected:
        virtual bool OnInitialize() = 0;
        virtual void OnTerminate() = 0;

        VulkanInstance* GetVulkanInstanceController() const
        { return _vulkanInstance; }

    private:
        static VulkanInstance* _vulkanInstance;
        
        std::list<BaseVulkanController*> _dependentControllers;
        BaseVulkanController* _parentController = nullptr;
    };
}
