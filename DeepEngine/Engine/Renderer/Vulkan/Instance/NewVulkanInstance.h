#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <set>

#include "../VulkanPCH.h"

namespace DeepEngine::Renderer::Vulkan
{

	// -----------------------------------------
	// FEATURES
	// -----------------------------------------

	struct FeatureInstanceExtension
	{
		VkExtensionProperties Extension;
		uint32_t MinVersion = 0;
	};

	struct FeaturePhysicalExtension
	{
		VkExtensionProperties Extension;
		uint32_t MinVersion = 0;

		// OR
		uint32_t FeatureFlagOffset;
	};

	struct FeatureQueueFlags
	{
		VkQueueFlagBits Flags;
		bool RequireSupportSurfaces;
	};

	struct FeaturePresenter
	{
		VkSurfaceFormat2KHR Format;
		VkPresentModeKHR Present;
	};

	struct Feature
	{
		virtual ~Feature() = default;

		constexpr virtual bool IsRequired() const
		{ return false; }

		// Add checking if feature was already declared in runtime (by sType)
		constexpr virtual void* GetInstanceFeaturePtr() const
		{ return nullptr; }
		
		constexpr virtual void* GetLogicalFeaturePtr() const
		{ return nullptr; }
		
		constexpr virtual std::vector<FeatureInstanceExtension> GetRequiredInstancesExtensions() const
		{ return { }; }
		
		constexpr virtual std::vector<FeaturePhysicalExtension> GetRequiredPhysicalExtensions() const
		{ return { }; }
		
		constexpr virtual std::vector<FeatureQueueFlags> GetRequiredQueues() const
		{ return { }; }
		
		constexpr virtual std::vector<FeatureInstanceExtension> GetRequiredLogicaExtensions() const
		{ return { }; }
	};

	// ---------------------------------------------------
	// FACTORY
	// ---------------------------------------------------

	struct AppDescription
	{
		std::string ApplicationName;
		uint32_t ApplicationVersion;
		std::string EngineName;
		uint32_t EngineVersion;
		uint32_t ApiVersion;
	};

	struct DebugDescription
	{
		std::vector<const char*> EnabledLayers;
	};

	struct GPUInfo
	{
		uint32_t ID;
		VkPhysicalDeviceProperties2 Properties;
		VkPhysicalDeviceFeatures2 Feaures;
	};
	
	// ---------------------------------------------------
	// DATA
	// ---------------------------------------------------

	// Move swapchain out!
	struct SwapChainData
	{
		// Format
		// Frame Buffer Size
		// Imgaes
		// Imges Views (???)
	};

	struct GPUData
	{
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;
	};
	
	struct InstanceData
	{
		VkInstance m_InstanceHandler;
		// HACKME
		// std::vector<VkExtensionProperties> m_Extensions ??
	};
	
	struct SurfaceData
	{
		VkSurfaceKHR m_SurfaceHandler;
		VkSurfaceCapabilitiesKHR m_Capabilities;
		VkSurfaceFormatKHR m_Format;
		VkPresentModeKHR m_PresentMode;
		uint8_t m_ImagesCount;
		bool m_RequireRecreate;
	};

	struct QueueData
	{
		std::vector<VkQueue> m_QueueHandlers;
											   
		std::vector<VkQueueFamilyProperties> m_FamilyProperties;
		uint32_t m_FamilyIndex;
		VkQueueFlagBits m_Bits;
		bool m_SupportsSurface;
	};
	

	struct InstanceDescription
	{
		struct FeatureData
		{
			std::unique_ptr<Feature> m_Feature;
			const std::type_info& m_Type;
		};
		
		std::vector<FeatureData> m_Features;
		AppDescription m_AppDesc;
		DebugDescription m_DebugDesc;

		InstanceDescription()
		{
			m_Features.reserve(32);
		}
		
		template <typename TFeature>
		requires std::is_base_of_v<Feature, TFeature>
		constexpr void AddFeature()
		{
			m_Features.emplace_back(std::make_unique<TFeature>(), typeid(TFeature));
		}

		template <typename TFeature>
		requires std::is_base_of_v<Feature, TFeature>
		constexpr void AddFeature(std::unique_ptr<Feature> p_feature)
		{
			m_Features.emplace_back(std::move(p_feature), typeid(TFeature));
		}
	};

	class InstanceController
	{
	public:
		static InstanceController Create(const InstanceDescription& p_desc)
		{
			
		}
		
		template <typename T>
		bool IsFeatureEnabled() const
		{
			type_info& type = typeid(T);
			return m_enabledFeatures.contains(type);
		}
		
	private:
		InstanceData m_instanceData;
		GPUData m_gpuData;
		SwapChainData m_swapChainData;
		SurfaceData m_surfaceData;
		std::vector<QueueData> m_queuesData;
		std::set<std::type_info&> m_enabledFeatures;
	};
	
}

