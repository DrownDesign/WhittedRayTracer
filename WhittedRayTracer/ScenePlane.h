#pragma once
#include "SceneObject.h"
#include <glm/glm.hpp>

using namespace glm;

class ScenePlane : public SceneObject
{
	ScenePlane(vec3 vertices, int *vertIndices, int numTriangles, vec2 uvs);
};

