#pragma once
#include <vector>
#include <memory>
#include <set>

#include "../VulkanPCH.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

	// -----------------------------------------
	// FEATURES
	// -----------------------------------------

	struct FeatureInstanceExtension
	{
		const char* ExtensionName;
		uint32_t MinVersion = 0;
	};

	struct FeaturePhysicalExtension
	{
		const char* ExtensionName;
		uint32_t MinVersion = 0;

		// OR
		uint32_t FeatureFlagOffset;
	};

	struct FeatureQueueFlags
	{
		VkQueueFlagBits Flags;
		bool RequireSupportSurfaces;
		uint32_t Count;
	};

	struct FeaturePresenter
	{
		VkSurfaceFormat2KHR Format;
		VkPresentModeKHR Present;
	};

	enum FeatureType
	{
		UNDECLARED = 0,
	};

	struct Feature
	{
		FeatureType m_Type;
		bool m_IsRequired;

		std::vector<FeatureInstanceExtension> m_RequiredInstancesExtensions;
		std::vector<FeaturePhysicalExtension> m_RequiredPhysicalExtensions;
		std::vector<FeatureQueueFlags> m_RequiredQueues;
		std::vector<FeatureInstanceExtension> m_RequiredLogicalExtensions;
		
		// Add checking if feature was already declared in runtime (by sType)
		std::vector<void*> m_InstancesFeaturesPointers;
		std::vector<void*> m_LogicalFeaturesPointers;
	};

	// ---------------------------------------------------
	// FACTORY
	// ---------------------------------------------------

	struct AppDescription
	{
		std::string m_ApplicationName;
		uint32_t m_ApplicationVersion;
		std::string m_EngineName;
		uint32_t m_EngineVersion;
		uint32_t m_ApiVersion;
	};

	struct DebugDescription
	{
		VulkanMessageLevel m_LogLevels;
		VulkanMessageType m_LogTypes;
		std::vector<const char*> m_EnabledLayers;
	};

	struct GPUInfo
	{
		uint32_t m_ID;
		VkPhysicalDeviceProperties2 m_Properties;
		VkPhysicalDeviceFeatures2 m_Feaures;
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
		// HACKME (Kostek) are enabled extensions really required?
		// std::vector<VkExtensionProperties> m_Extensions
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

	struct LayersDebugData
	{
		VulkanMessageLevel m_LogLevels;
		VulkanMessageType m_LogTypes;
		std::vector<VkLayerProperties> m_EnabledLayers;
	};
	

	struct CoreDescription
	{
		std::vector<Feature*> m_Features;
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
		friend class CoreControllerFactory;
		
	public:
		
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

	class CoreControllerFactory
	{
	private:
		struct FeatureData
		{
			Feature* const m_Feature;
			bool m_IsAvailable;
		};
		
	public:
		CoreControllerFactory() = delete;
		
		static std::shared_ptr<CoreController> Create(const CoreDescription& p_desc)
		{
			std::vector<FeatureData> features(p_desc.m_Features.size());

			for (uint32_t i = 0; i < features.size(); i++)
			{
				features.emplace_back(p_desc.m_Features[i], true);
			}

			auto instanceExtensions = GetInstanceExtensions();
			CheckFeaturesForInstanceExtensions(features, instanceExtensions);

			if (!AreAllRequiredFeaturesAvailable(features))
			{
				return nullptr;
			}
			
			
			// Get available instance extensions
			// Compare extensions with features
			// Return null if required features has not been matched

			// Get available Physical devices
			// Get availabel queues per physiacl devices
			// Find device with matching all required features and the larges amount of optional features

			// Create debug layers create info
			// Create Instance
			// Create Debug messenger

			// Create LogicalDevice
			// Create Queues

			return nullptr;
		}

	private:
		static std::vector<VkExtensionProperties> GetInstanceExtensions()
		{
			uint32_t count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

			auto extensions = std::vector<VkExtensionProperties>(count);
			vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

			VULKAN_TRACE("Found {0} instance extensions:", count);
			for (int i = 0; i < count; i++)
			{
				VULKAN_TRACE("\t{:<45} (v.{})", extensions[i].extensionName, extensions[i].specVersion);
			}

			return extensions;
		}

		static void CheckFeaturesForInstanceExtensions(std::vector<FeatureData>& p_features,
			const std::vector<VkExtensionProperties>& p_extensions)
		{
			for (auto& feature : p_features)
			{
				if (!feature.m_IsAvailable)
				{
					continue;
				}
				
				for (auto& requiredExtension : feature.m_Feature->m_RequiredInstancesExtensions)
				{
					auto* extension = GetExtensionByName(requiredExtension.ExtensionName, p_extensions);

					if (extension != nullptr && extension->specVersion >= requiredExtension.MinVersion)
					{
						if (feature.m_Feature->m_IsRequired)
						{
							VULKAN_ERR("Required Extension \"{}\" in version is not supported on current machine!!", extension->extensionName);
						}
						else
						{
							VULKAN_WARN("Extension \"{}\" in version is not supported on current machine.", extension->extensionName);
						}
						
						feature.m_IsAvailable = false;
					}
				}
			}
		}

		static const VkExtensionProperties* GetExtensionByName(const char* p_name,
			const std::vector<VkExtensionProperties>& p_extensions)
		{
			for (auto& extension : p_extensions)
			{
				if (std::string(extension.extensionName) == std::string(p_name))
				{
					return &extension;
				}
			}

			return nullptr;
		}

		static bool AreAllRequiredFeaturesAvailable(const std::vector<FeatureData>& p_features)
		{
			for (auto feature : p_features)
			{
				if (feature.m_Feature->m_IsRequired && !feature.m_IsAvailable)
				{
					return false;
				}
			}

			return true;
		}
	};
	
}

