#include "ScenePlane.h"

ScenePlane::ScenePlane(vec3 vertices, int *vertIndex, int numTriangles, vec2 uvs) 
{
	int maxIndex = 0;
	for (int i = 0; i < numTriangles * 3; i++) {
		if (vertIndex[i] > maxIndex) {
			maxIndex = vertIndex[i];
		}
		maxIndex += 1;
		vertices = vec3(maxIndex);
		/*Work here*/
	}
};
