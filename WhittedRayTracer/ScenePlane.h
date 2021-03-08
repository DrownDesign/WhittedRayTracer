#pragma once
#include "SceneObject.h"
#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using namespace std;

class ScenePlane : public SceneObject
{
	vector<vec3> vertices;
	int numTris;
	vector<int> indices;
	vector<vec2> uvCoords;

	ScenePlane(vec3 verts, int vertIndices, int numTriangles, vec2 uvs);

	bool rayTriangleIntersect(vec3 v0, vec3 v1, vec3 v2, vec3 origin, vec3 direction, float tnear, float u, float v) const;

	//Is the ray intersecting with the plane
	bool interstect(vec3 origin, vec3 direction, float tnear, int index, vec2 uv) const;

	//Get surface properties of the plane
	void getSurfaceProperties(vec3 p, vec3 i, int index, vec2 uv, vec3 n, vec2 st) const;	

	//Evaluate the diffuse color of the given plane
	vec3 evalDiffuse(vec2 st) const;
};

