#pragma once
#include <list>
#include <memory>

namespace DeepEngine::Renderer::Vulkan
{
    class VulkanInstance;
    
    class BaseVulkanController
    {
    public:
        BaseVulkanController() = delete;
        
        // TODO (Kostek): Replace double pointer with some kind of reference
        template <typename T>
        requires std::is_base_of_v<BaseVulkanController, T>
        bool TryCreateSubController(T** p_outputPtr)
        {
            auto* ptr = new T(_vulkanInstance);
            _subInstances.push_back(ptr);
            
            ptr->_pointerInContainer = &_subInstances.back();

            *p_outputPtr = ptr;
            return ptr->OnInitialized();
        }
        
    protected:
        BaseVulkanController(VulkanInstance* p_vulkanInstance);
        virtual ~BaseVulkanController();

    protected:
        virtual bool OnInitialized() = 0;
        virtual void OnTerminate() = 0;

        VulkanInstance* GetVulkanInstanceController() const
        { return _vulkanInstance; }

    private:
        VulkanInstance* _vulkanInstance;
        
        BaseVulkanController** _pointerInContainer = nullptr;
        std::list<BaseVulkanController*> _subInstances;
    };
    
}
