#pragma once
#include <memory>
#include <stack>

namespace DeepEngine::Renderer::Vulkan
{
    class VulkanInstance;
    
    class BaseVulkanController
    {
        friend class VulkanInstance;
        
    public:
        // TODO (Kostek): Replace double pointer with some kind of reference
        template <typename T, typename ...Args>
        requires std::is_base_of_v<BaseVulkanController, T>
        bool TryCreateSubController(T** p_outputPtr, Args... p_args)
        {
            auto* ptr = new T(p_args...);
            _subInstances.push(ptr);
            
            ptr->_pointerInContainer = &_subInstances.top();

            *p_outputPtr = ptr;
            return ptr->OnInitialized();
        }
        
    protected:
        BaseVulkanController() = default;
        virtual ~BaseVulkanController();

    protected:
        virtual bool OnInitialized() = 0;
        virtual void OnTerminate() = 0;

        VulkanInstance* GetVulkanInstanceController() const
        { return _vulkanInstance; }

    private:
        static VulkanInstance* _vulkanInstance;
        
        BaseVulkanController** _pointerInContainer = nullptr;
        std::stack<BaseVulkanController*> _subInstances;
    };
    
}
