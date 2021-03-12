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

	SceneObject() :
		materialType(DIFFUSE_AND_GLOSSY),
		ior(1.3), kd(0.8), ks(0.2), diffuseColor(0.2), specular(25) {}

	virtual ~SceneObject() {}

	virtual bool intersect(const vec3 &orig, const vec3 &dir, float &tnear, int &index, vec2 &uv) const = 0;
	virtual void getSurfaceProperties(const vec3 &p, const vec3 &i, const int &index, const vec2 &uv, vec3 &n, vec2 &st) const = 0;
	virtual vec3 evalDiffuseColor(const vec2 &) { return diffuseColor; }
};
