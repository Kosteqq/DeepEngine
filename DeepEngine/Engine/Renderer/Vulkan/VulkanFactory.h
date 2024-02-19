#pragma once
#include "VulkanObject.h"
#include "Instance/VulkanInstance.h"


namespace DeepEngine::Engine::Renderer::Vulkan
{

    template <typename T>
    concept VulkanObjectKind = std::is_base_of_v<VulkanObject, T>;

    template <VulkanObjectKind T>
    using VulkanRef = std::shared_ptr<T>;
    
    class VulkanFactory
    {
        template <VulkanObjectKind T>
        class SubFactory;
        
        template <VulkanObjectKind T>
        friend class SubFactory;
        
    public:
        template <VulkanObjectKind T>
        using FactoryOf = SubFactory<T>;
        
        VulkanFactory(VulkanInstance& p_vulkanInstance);

        void Bind();
        static void TerminateObject(const std::shared_ptr<VulkanObject>& p_object);

    private:
        static void TerminateObject(VulkanObject* p_object);
        static void DestroyPointerHandler(VulkanObject* p_object);
    
        template <VulkanObjectKind T, VulkanObjectKind ParentType>
        static std::shared_ptr<T> CreateObject(T* p_instance, VulkanObject::TerminateFunction p_terminateFunc,
            const std::shared_ptr<ParentType>& p_parentObject = nullptr)
        {
            p_instance->_terminateFunc = p_terminateFunc;
            std::shared_ptr<T> ptr = std::shared_ptr<T>(p_instance, DestroyPointerHandler);

            if (p_parentObject != nullptr)
            {
                p_parentObject->_subobjects.push_back(std::weak_ptr<T>(ptr));
            }
            
            return ptr;
        }

    private:
        static VulkanFactory* _bindFactory;
        VulkanInstance& _vulkanInstance;
    };
    
}
