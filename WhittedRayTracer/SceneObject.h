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

	SceneObject() : materialType(DIFFUSE_AND_GLOSSY), ior(1.3), kd(0.8), ks(0.2), diffuseColor(0.2), specular(25) {}

	bool intersect(const vec3 &, const vec3 &, float &, int &, vec2 &);
	void getSurfaceProperties(const vec3 &, const vec3 &, const int &, const vec2 &, vec3 &, vec2 &);
	vec3 evalDiffuseColor(const vec2 &) { return diffuseColor; }
};
