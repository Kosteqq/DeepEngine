#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#define NOMINMAX
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "VulkanCommandPool.h"

namespace DeepEngine::Renderer
{
    class VertexBuffer
    {
    public:
        struct Vertex
        {
            glm::vec2 pos;
            glm::vec3 color;

            static constexpr VkVertexInputBindingDescription GetBindingDesc()
            {
                VkVertexInputBindingDescription description { };
                description.binding = 0;
                description.stride = sizeof(Vertex);
                description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                
                return description;
            }

            static constexpr std::array<VkVertexInputAttributeDescription, 2> GetAttributeDesc()
            {
                std::array<VkVertexInputAttributeDescription, 2> descs { };
                descs[0].binding = 0;
                descs[0].location = 0; // shader location
                descs[0].format = VK_FORMAT_R32G32_SFLOAT;
                descs[0].offset = 0;

                descs[1].binding = 0;
                descs[1].location = 1;
                descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                descs[1].offset = sizeof(glm::vec2);
                
                return descs;
            }
        };
        
    public:
        VertexBuffer(std::shared_ptr<DeepEngine::Debug::Logger> p_logger, VulkanLogicalLayer* p_logicalLayer,
            VulkanPhysicalLayer* p_physicalLayer)
            : _logger(p_logger), _logicalLayer(p_logicalLayer), _physicalLayer(p_physicalLayer)
        {
            
        }

        ~VertexBuffer()
        {
            Terminate();
        }

        bool Init()
        {
            _bufferSize = sizeof(Vertex) * vertices.size();
            
            if (!CreateBuffer(
                _bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                _stagingBuffer,
                _stagingBufferMemory))
            {
                return false;
            }
            
            void* data;
            vkMapMemory(_logicalLayer->GetLogicalDevice(), _stagingBufferMemory, 0, _bufferSize, 0, &data);
            memcpy(data, vertices.data(), _bufferSize);
            vkUnmapMemory(_logicalLayer->GetLogicalDevice(), _stagingBufferMemory);

            if (!CreateBuffer(
                _bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                _buffer,
                _bufferMemory))
            {
                return false;
            }

            // p_commandPool->CopyBuffer(stagingBuffer, _buffer, bufferSize, p_queue);
            //
            // vkDestroyBuffer(_logicalLayer->GetLogicalDevice(), stagingBuffer, nullptr);
            // vkFreeMemory(_logicalLayer->GetLogicalDevice(), stagingBufferMemory, nullptr);

            return true;
        }

        void Terminate()
        {
            vkDestroyBuffer(_logicalLayer->GetLogicalDevice(), _stagingBuffer, nullptr);
            vkFreeMemory(_logicalLayer->GetLogicalDevice(), _stagingBufferMemory, nullptr);
            
            vkDestroyBuffer(_logicalLayer->GetLogicalDevice(), _buffer, nullptr);
            vkFreeMemory(_logicalLayer->GetLogicalDevice(), _bufferMemory, nullptr);
        }

        VkBuffer GetBuffer() const
        { return _buffer; }

        VkBuffer GetStagingBuffer() const
        { return _stagingBuffer; }

        VkDeviceSize GetBufferSize() const
        { return _bufferSize; }

        uint32_t GetVertexCount() const
        { return vertices.size(); }

    private:
        bool CreateBuffer(VkDeviceSize p_size, VkBufferUsageFlags p_usage, VkMemoryPropertyFlags p_properties,
            VkBuffer& p_buffer, VkDeviceMemory& p_bufferMemory)
        {
            VkBufferCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            createInfo.size = p_size;
            createInfo.usage = p_usage;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            auto result = vkCreateBuffer(_logicalLayer->GetLogicalDevice(), &createInfo, nullptr, &p_buffer);
            if (result != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create vertex buffer with return result {}", string_VkResult(result));
                return false;
            }

            VkMemoryRequirements memReq;
            vkGetBufferMemoryRequirements(_logicalLayer->GetLogicalDevice(), p_buffer, &memReq);

            VkMemoryAllocateInfo allocInfo { };
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memReq.size;
            // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT - for sure that memory will be send to the GPU
            allocInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            result = vkAllocateMemory(_logicalLayer->GetLogicalDevice(), &allocInfo, nullptr, &p_bufferMemory);
            if (result != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to allocate vertex buffer memory, result: {}", string_VkResult(result));
                return false;
            }

            vkBindBufferMemory(_logicalLayer->GetLogicalDevice(), p_buffer, p_bufferMemory, 0);
            return true;
        }
        
        int32_t FindMemoryType(uint32_t p_type, VkMemoryPropertyFlags p_properties)
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(_physicalLayer->GetDevice(), &memProperties);

            for (int32_t i = 0; i < memProperties.memoryTypeCount; i++)
            {
                if ((p_type & (1 << i)) != 0)
                {
                    if ((memProperties.memoryTypes[i].propertyFlags & p_properties) == p_properties)
                    {
                        return i;
                    }
                }
            }

            return -1;
        }

    private:
        const std::vector<Vertex> vertices = {
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}
        };

        std::shared_ptr<DeepEngine::Debug::Logger> _logger;
        VulkanLogicalLayer* _logicalLayer;
        VulkanPhysicalLayer* _physicalLayer;

        VkBuffer _stagingBuffer;
        VkDeviceMemory _stagingBufferMemory;

        VkDeviceSize _bufferSize;
        VkBuffer _buffer;
        VkDeviceMemory _bufferMemory;
    };
}
