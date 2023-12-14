
W celu łatwiejszej implementacji i zarządzaniem featerami vulkana są tworzone wrappery w subfolderze renderera `Vulkan` zagnieżdżone w namespace o takiej samej nazwie (pełne rozwinięcie to `DeepEngine::Renderer::Vulkan`)

## Template Implementacji

```
#pragma once  

// Prekompilowany header vulkana
#include "VulkanPCH"

#include "Controller/BaseVulkanController.h"

namespace DeepEngine::Renderer::Vulkan  
{  

	// Przyjmijmy że "Feature" reprezentuje klasę Vulkana jak VkFence czy VkLogicalDevice
	class Feature : BaseVulkanController  
    {  
    public:  
	    // W konstruktorze możesz podać różne parametry, czy odwołania do innych featerów
        Feature() { }
        ~Feature() { }
        
        VkFeature GetVkFeature() const  
        { return _feature; } 
  
    protected:  
        bool OnInitialize() override  
        {  
	        // ...
	        
	        // Zastąp vkCreateFeature oraz createInfo odpowiednikami
			VULKAN_CHECK_CREATE(  
				vkCreateFeature(
					GetVulkanInstanceController()->GetLogicalDevice(),
					&createInfo,
					nullptr,
					&_feature),  
				"Wiadomość wyświetlona w razie nie powodzenia")  
  
            return true;  
        }  
        void OnTerminate() override  
        {  
	        // Kod wykonywany w trakcie niszczecznia
	        VkDestroyFeature(GetVulkanInstanceController()->GetLogicalDevice(), _feature, nullptr);
        }  
        
    private:  
        VkFeature _feature = VK_NULL_HANDLE;  
    };
}
```

## Tworzenie Instancji
Tworzenie instancji feature-a polega na utworzeniu jej przy pomocą keyworda `new` i wywołaniu funkcji `InitializeSubController` w instancji klasy powiązanej.
Przykładowo tworząc CommandBuffer powinniśmy wywołać `InitializeSubController`  na instancji klasy CommandPool, ponieważ utworzenie CommandBuffer musiało nastąpić po nim i jego istnienie jest powiązane.
Gdy klasa nie jest powiązana z niczym (przykładem może być w.w. CommandPool) funkcja ta powinna zostać wywołana na instancji VulkanInstance