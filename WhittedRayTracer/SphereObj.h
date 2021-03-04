#pragma once
#include <glm/glm.hpp>
#include "SceneObject.cpp"

using namespace glm;

class SphereObj : public SceneObject
{
public:
	vec3 center;
	float rad, rad2;


};

