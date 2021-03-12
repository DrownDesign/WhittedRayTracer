#include "SphereObj.h"

SphereObj::SphereObj(const vec3 & c, const float & r) : center(c), rad(r), rad2(r*r) {}

bool SphereObj::intersect(const vec3 &p1, const vec3 &dir, float &tnear, int &index, vec2 &uv) const
{
	vec3 L = p1 - center;
	float a = dot(dir, dir);
	float b = 2 * dot(dir, L);
	float c = dot(L, L) - rad2;

	float t0, t1;

	if (!solveQuad(a, b, c, t0, t1)) return false;

	if (t0 < 0) t0 = t1;
	if (t0 < 0) return false;
	tnear = t0;

	return true;
}

void SphereObj::getSurfaceProperties(const vec3 &p, const vec3 &i, const int &index, const vec2 &uv, vec3 &n, vec2 &st) const
{
	n = normalize(p - center); 
}

bool SphereObj::solveQuad(const float & a, const float & b, const float & c, float & x0, float & x1) const
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