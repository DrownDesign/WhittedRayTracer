#include "SphereObj.h"

SphereObj::SphereObj(const vec3 & c, const float & r) : center(c), rad(r), rad2(r*r) {}

bool SphereObj::intersect(const vec3 & p1, const vec3 & dir, float & tnear, int & index, vec2 & uv)
{
	vec3 L = p1 - center;
	float a = dot(dir, dir);
	float b = 2 * dot(dir, L);
	float c = dot(L, L) - rad2;
	
	float t0, t1;

	if(!solveQuad(a, b, c, t0, t1)) return false;

	if (t0 < 0) t0 = t1;
	if (t0 < 0) return false;
	tnear = t0;

	return true;
}