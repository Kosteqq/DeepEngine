#pragma once
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

	using FeatureType = std::type_info;

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

		std::set<const FeatureType&> m_SupportFeatures;
	};
	

	struct CoreDescription
	{
		struct FeatureData
		{
			std::unique_ptr<Feature> m_Feature;
			const FeatureType& m_Type;
		};
		
		std::vector<FeatureData> m_Features;
		AppDescription m_AppDesc;
		DebugDescription m_DebugDesc;

		CoreDescription()
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

	class CoreController
	{
	public:
		static CoreController Create(const CoreDescription& p_desc)
		{
			
		}
		
		template <typename TFeature>
		requires std::is_base_of_v<Feature, TFeature>
		bool IsFeatureEnabled() const
		{
			const FeatureType& type = typeid(TFeature);
			return m_enabledFeatures.contains(type);
		}

		template <typename TFeature>
		requires std::is_base_of_v<Feature, TFeature>
		std::vector<QueueData> GetFeatureSupportQueues() const
		{
			std::vector<QueueData> output;
			output.reserve(m_queuesData.size());
			
			const FeatureType& type = typeid(TFeature);
			
			for (uint32_t i = 0; i < m_queuesData.size(); i++)
			{
				if (m_queuesData[i].m_SupportFeatures.contains(type))
				{
					output.push_back(m_queuesData[i]);
				}
			}
			return output;
		}
		
	private:
		InstanceData m_instanceData;
		GPUData m_gpuData;
		SwapChainData m_swapChainData;
		SurfaceData m_surfaceData;
		std::vector<QueueData> m_queuesData;
		std::set<const FeatureType&> m_enabledFeatures;
	};
	
}

