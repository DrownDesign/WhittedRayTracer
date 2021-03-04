#pragma once
#include <glm/glm.hpp>

class SceneLight
{
public:

	glm::vec3 pos;
	glm::vec3 intensity;

	SceneLight(const glm::vec3 &p, const glm::vec3 &i);
};

