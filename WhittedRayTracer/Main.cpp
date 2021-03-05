//Other Classes
#include "SceneObject.h"
#include "SphereObj.h"
#include "SceneLight.h"

//Libraries
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

using namespace std;
using namespace glm;

//Defines the environment objects and lights
void defineScene() {

	vector<unique_ptr<SceneObject>> sceneObjs;

	//Sphere 1 properties
	SphereObj *sph1 = new SphereObj(vec3(-1, 0, -12), 2);
	sph1->materialType = DIFFUSE_AND_GLOSSY;
	sph1->diffuseColor = vec3(0.6, 0.7, 0.8);

	//Sphere 2 properties
	SphereObj *sph2 = new SphereObj(vec3(0.6, 0.7, 0.8), 1.5);
	sph2->materialType = REFLECTION_AND_REFRACTION;
	sph2->ior = 1.5;

	sceneObjs.push_back(unique_ptr<SphereObj>(sph1));
	sceneObjs.push_back(unique_ptr<SphereObj>(sph2));

	printf("Scene Defined Successfully");
}

//Runs on startup
int main(int argc, char **argv) {
	printf("Is Running");

	defineScene();

	exit(0);
}

