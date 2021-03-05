#include "SceneObject.h"
#include <utility>

SceneObject::SceneObject() : materialType(DIFFUSE_AND_GLOSSY), ior(1.3f), kd(0.8f), ks(0.2f), diffuseColor(0.2f), specular(25.0f) {}


bool SceneObject::intersect(const vec3 &, const vec3 &, float &, int &, vec2 &)
{
	return false;
}

void SceneObject::getSurfaceProperties(const vec3 &, const vec3 &, const int &, const vec2 &, vec3 &, vec2 &)
{
}

bool SceneObject::solveQuad(const float & a, const float & b, const float & c, float & x0, float & x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = -0.5f * b / a;
	else {
		float q = (b > 0) ?
			-0.5f * (b + sqrt(discr)) :
			-0.5f * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}

	if (x0 > x1) std::swap(x0, x1);
	return true;
}
