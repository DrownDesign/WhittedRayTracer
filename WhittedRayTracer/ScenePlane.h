#pragma once
#include "SceneObject.h"
#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using namespace std;

class ScenePlane : public SceneObject
{
public:
	vector<vec3> vertices;
	int numTris;
	vector<int> indices;
	vector<vec2> stCoords;

	ScenePlane(vector<vec3> verts, vector<int> vertIndices, int numTriangles, vector<vec2> st);

	bool rayTriangleIntersect(const vec3 &v0, const vec3 &v1, const vec3 &v2, const vec3 &origin, const vec3 &direction, float &tnear, float &u, float &v) const;

	//Is the ray intersecting with the plane
	bool intersect(const vec3 &orig, const vec3 &dir, float &tnear, int &index, vec2 &uv) const;


	//Get surface properties of the plane
	void getSurfaceProperties(const vec3 &p, const vec3 &i, const int &index, const vec2 &uv, vec3 &n, vec2 &st) const;

	//Evaluate the diffuse color of the given plane
	vec3 evalDiffuseColor(const vec2 &st);
};

