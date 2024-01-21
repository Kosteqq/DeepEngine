#pragma once
#include <glm/vec2.hpp>

#include "Core/Events/BaseEvent.h"

namespace DeepEngine::Engine::Renderer::Events
{

	BEGIN_LOCAL_EVENT_DEFINITION(OnViewportResized)
	glm::vec2 NewViewportSize;
	END_EVENT_DEFINITION
	
}

