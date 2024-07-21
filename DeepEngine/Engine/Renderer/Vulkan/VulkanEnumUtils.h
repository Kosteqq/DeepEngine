#pragma once

#define DECLARE_ENUM_OPERATORS(Enum)        \
constexpr Enum operator~ (Enum a)           \
{                                           \
 return (Enum)~(int)a;                      \
}                                           \
constexpr Enum operator| (Enum a, Enum b)   \
{                                           \
 return (Enum)((int)a | (int)b);            \
}                                           \
constexpr Enum operator& (Enum a, Enum b)   \
{                                           \
 return (Enum)((int)a & (int)b);            \
}                                           \
constexpr Enum operator^ (Enum a, Enum b)   \
{                                           \
 return (Enum)((int)a ^ (int)b);            \
}                                           \
constexpr Enum& operator|= (Enum& a, Enum b)\
{                                           \
 return (Enum&)((int&)a |= (int)b);         \
}                                           \
constexpr Enum& operator&= (Enum& a, Enum b)\
{                                           \
 return (Enum&)((int&)a &= (int)b);         \
}                                           \
constexpr Enum& operator^= (Enum& a, Enum b)\
{                                           \
 return (Enum&)((int&)a ^= (int)b);         \
}

namespace DeepEngine::Engine::Renderer::Vulkan
{
    enum VulkanMessageLevel
    {
        NONE_LEVEL = 0x00000000,
        TRACE = VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        INFO = VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
        WARN = VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
        ERR = VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        ALL_LEVELS = VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT,
    };

    // HACKME (Kostek) Check is this require
    // constexpr operator VkDebugUtilsMessageSeverityFlagBitsEXT(VulkanMessageLevel p_messageLevel)
    // {
    //     return (VkDebugUtilsMessageSeverityFlagBitsEXT)(int)p_messageLevel;
    // }
 
    enum VulkanMessageType
    {
        NONE_TYPE = 0x00000000,
        GENERAL = VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
        VALIDATION = VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
        PERFORMANCE  = VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        DEVICE_ADDRESS_BINDING = VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
        ALL_TYPES = VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT,
    };
 
    // HACKME (Kostek) Check is this require
    // constexpr operator VkDebugUtilsMessageTypeFlagBitsEXT(VulkanMessageType p_messageLevel)
    // {
    //     return (VkDebugUtilsMessageTypeFlagBitsEXT)(int)p_messageLevel;
    // }
}

DECLARE_ENUM_OPERATORS(VkDebugUtilsMessageSeverityFlagBitsEXT)
DECLARE_ENUM_OPERATORS(VkDebugUtilsMessageTypeFlagBitsEXT)
DECLARE_ENUM_OPERATORS(DeepEngine::Engine::Renderer::Vulkan::VulkanMessageLevel)
DECLARE_ENUM_OPERATORS(DeepEngine::Engine::Renderer::Vulkan::VulkanMessageType)
