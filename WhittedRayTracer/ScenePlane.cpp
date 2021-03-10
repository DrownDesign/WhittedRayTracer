#include "ScenePlane.h"
#include "Ray.h"
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include <utility>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cmath>

//This may be full of errors. Double check when running
ScenePlane::ScenePlane(vector<vec3> verts, vector<int> vertIndex, int numTriangles, vector<vec2> uvs) 
{
	vertices.clear();
	indices.clear();
	uvCoords.clear();

	int maxIndex = 0;

	for (int i = 0; i < numTriangles * 3; i++) {
		
		if (vertIndex.at(i) > maxIndex) {
			maxIndex = vertIndex.at(i);
		}
		
		maxIndex += 1;
		vertices = verts;
		indices = vertIndex;
		numTris = numTriangles;
		uvCoords = uvs;

	}
}

bool ScenePlane::interstect(vec3 origin, vec3 direction, float tnear, int index, vec2 uv) const
{
	bool intersect = false;

	for (int i = 0; i < numTris; i++) {
		//Double check indices logic when debugging.
		vec3 v0 = vertices[indices[i * 3]];
		vec3 v1 = vertices[indices[i * 3 + 1]];
		vec3 v2 = vertices[indices[i * 3 + 2]];
		float t = 0, u = 0, v = 0;

		if (rayTriangleIntersect(v0, v1, v2, origin, direction, t, u ,v) && t < tnear) {
			tnear = t;
			uv.x = u;
			uv.y = v;
			index = i;
			intersect |= true;
		}
	}

	return intersect;
}

bool ScenePlane::rayTriangleIntersect(vec3 v0, vec3 v1, vec3 v2, vec3 origin, vec3 direction, float tnear, float u, float v) const {
	//Define Edges
	vec3 edge1 = v1 - v0;
	vec3 edge2 = v2 - v0;

	//Get cross product of direction vector and second edge
	vec3 pvec = cross(direction, edge2);
	float determinant = dot(edge1, pvec);
	if (determinant == 0 || determinant < 0) return false;

	vec3 tvec = origin - v0;
	u = dot(tvec, pvec);
	if (u < 0 || u > determinant) return false;

	vec3 qvec = cross(tvec, edge1);
	v = dot(direction, qvec);
	if (v < 0 || u + v > determinant) return false;

	float inverseDet = 1 / determinant;

	tnear = dot(edge2, qvec) * inverseDet;
	u *= inverseDet;
	v *= inverseDet;

	return true;
};

void ScenePlane::getSurfaceProperties(vec3 p, vec3 i, int index, vec2 uv, vec3 n, vec2 st) const
{
	vec3 v0 = vertices[indices[index * 3]];
	vec3 v1 = vertices[indices[index * 3 + 1]];
	vec3 v2 = vertices[indices[index * 3 + 2]];

	vec3 edge0 = normalize(v1 - v0);
	vec3 edge1 = normalize(v2 - v1);
	n = normalize(cross(edge0, edge1));

	vec2 st0 = uvCoords[indices[index * 3]];
	vec2 st1 = uvCoords[indices[index * 3 + 1]];
	vec2 st2 = uvCoords[indices[index * 3 + 2]];
	st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
}
vec3 ScenePlane::evalDiffuse(vec2 st) const
{
	float scale = .5;
	float pattern = (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);

	return mix(vec3(0.815,0.235, 0.031), vec3(0.937, 0.937, 0.231), pattern);
};
