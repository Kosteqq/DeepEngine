#include "VulkanPrototype.h"

namespace DeepEngine::Renderer
{
    std::shared_ptr<Core::Debug::Logger> VulkanPrototype::_debugVkLogger = Core::Debug::Logger::CreateLoggerInstance("VK Validation Layer");

}
