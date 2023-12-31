#pragma once
#include <glm/vec2.hpp>

#include "Architecture/EventBus/BaseEvent.h"

namespace DeepEngine::Renderer::Events
{

	BEGIN_LOCAL_EVENT_DEFINITION(OnViewportResized)
	glm::vec2 NewViewportSize;
	END_EVENT_DEFINITION
	
}

