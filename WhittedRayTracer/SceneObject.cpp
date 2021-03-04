#include "SceneObject.h"

SceneObject::SceneObject() : materialType(DIFFUSE_AND_GLOSSY), ior(1.3f), kd(0.8f), ks(0.2f), diffuseColor(0.2f), specular(25.0f) {}


bool SceneObject::intersect(const vec3 &, const vec3 &, float &, int &, vec2 &)
{
	return false;
}

void SceneObject::getSurfaceProperties(const vec3 &, const vec3 &, const int &, const vec2 &, vec3 &, vec2 &)
{
}
