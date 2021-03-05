#pragma once
#include <glm/glm.hpp>
#include "SceneObject.h"

using namespace glm;

class SphereObj : public SceneObject
{
public:
	vec3 center;
	float rad, rad2;

	SphereObj(const vec3 &c, const float &r);

	bool intersect(const vec3 &p1, const vec3 &dir, float &tnear, int &index, vec2 &uv);
};

