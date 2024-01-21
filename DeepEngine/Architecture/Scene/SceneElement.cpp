#include "SceneElement.h"

namespace DeepEngine::Architecture::Scene
{
	
	glm::mat4 Transform::GetLocalTransform() const
	{
		auto matrix = glm::mat4(1.f);
		matrix = glm::translate(matrix, Position);
		matrix = glm::rotate(matrix, Rotation.x, { 1, 0, 0 });
		matrix = glm::rotate(matrix, Rotation.y, { 0, 1, 0 });
		matrix = glm::rotate(matrix, Rotation.z, { 0, 0, 1 });
		matrix = glm::scale(matrix, Scale);
		return matrix;
	}
	
}

