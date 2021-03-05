#pragma once
enum MaterialType {DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION};
#include <glm/glm.hpp>

using namespace glm;

class SceneObject
{

public:

	//Material Properties
	MaterialType materialType;
	float ior;
	float kd, ks;
	vec3 diffuseColor;
	float specular;

	SceneObject();

	bool intersect(const vec3 &, const vec3 &, float &, int &, vec2 &);
	void getSurfaceProperties(const vec3 &, const vec3 &, const int &, const vec2 &, vec3 &, vec2 &);
	vec3 evalDiffuseColor(const vec2 &) { return diffuseColor; }
	bool solveQuad(const float &a, const float &b, const float &c, float &x0, float &x1);
};
