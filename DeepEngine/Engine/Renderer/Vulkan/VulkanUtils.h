#pragma once

constexpr VkDebugUtilsMessageTypeFlagBitsEXT operator~ (VkDebugUtilsMessageTypeFlagBitsEXT a)
{
 return (VkDebugUtilsMessageTypeFlagBitsEXT)~(int)a;
}

constexpr VkDebugUtilsMessageTypeFlagBitsEXT operator| (VkDebugUtilsMessageTypeFlagBitsEXT a, VkDebugUtilsMessageTypeFlagBitsEXT b) 
{
 return (VkDebugUtilsMessageTypeFlagBitsEXT)((int)a | (int)b); 
}

constexpr VkDebugUtilsMessageTypeFlagBitsEXT operator& (VkDebugUtilsMessageTypeFlagBitsEXT a, VkDebugUtilsMessageTypeFlagBitsEXT b) 
{
 return (VkDebugUtilsMessageTypeFlagBitsEXT)((int)a & (int)b); 
}

constexpr VkDebugUtilsMessageTypeFlagBitsEXT operator^ (VkDebugUtilsMessageTypeFlagBitsEXT a, VkDebugUtilsMessageTypeFlagBitsEXT b) 
{
 return (VkDebugUtilsMessageTypeFlagBitsEXT)((int)a ^ (int)b); 
}

constexpr VkDebugUtilsMessageTypeFlagBitsEXT& operator|= (VkDebugUtilsMessageTypeFlagBitsEXT& a, VkDebugUtilsMessageTypeFlagBitsEXT b)
{
 return (VkDebugUtilsMessageTypeFlagBitsEXT&)((int&)a |= (int)b); 
}

constexpr VkDebugUtilsMessageTypeFlagBitsEXT& operator&= (VkDebugUtilsMessageTypeFlagBitsEXT& a, VkDebugUtilsMessageTypeFlagBitsEXT b)
{
 return (VkDebugUtilsMessageTypeFlagBitsEXT&)((int&)a &= (int)b); 
}

constexpr VkDebugUtilsMessageTypeFlagBitsEXT& operator^= (VkDebugUtilsMessageTypeFlagBitsEXT& a, VkDebugUtilsMessageTypeFlagBitsEXT b)
{
 return (VkDebugUtilsMessageTypeFlagBitsEXT&)((int&)a ^= (int)b); 
}
