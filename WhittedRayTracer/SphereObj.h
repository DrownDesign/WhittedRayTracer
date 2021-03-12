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

	bool intersect(const vec3 &p1, const vec3 &dir, float &tnear, int &index, vec2 &uv) const;

	void getSurfaceProperties(const vec3 &p, const vec3 &i, const int &index, const vec2 &uv, vec3 &n, vec2 &st) const;

	bool solveQuad(const float &a, const float &b, const float &c, float &x0, float &x1) const;

};

