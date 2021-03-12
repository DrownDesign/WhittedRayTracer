//Other Classes
#include "SceneObject.h"
#include "SphereObj.h"
#include "SceneLight.h"
#include "ScenePlane.h"

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

const float INFINTY = numeric_limits<float>::max();

vector<unique_ptr<SceneObject>> sceneObjs;
vector<unique_ptr<SceneLight>> sceneLights;

//Defines the environment objects and lights
void defineScene() {

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

	//Define plane
	vector<vec3> vertices{ vec3(-5, -3, -6), vec3(5,-3,-6), vec3(5,-3,-16), vec3(-5,-3,-16) };
	vector<int> indices{ 0 , 1 , 3 , 1 , 2 , 3 };
	vector<vec2> uvs{ {0,0}, {1,0}, {1,1}, {0,1} };
	ScenePlane *plane = new ScenePlane(vertices, indices, 2, uvs);
	plane->materialType = DIFFUSE_AND_GLOSSY;

	sceneObjs.push_back(unique_ptr<ScenePlane>(plane));

	sceneLights.push_back(unique_ptr<SceneLight>(new SceneLight(vec3(-20, 70, 20), vec3(0.5))));
	sceneLights.push_back(unique_ptr<SceneLight>(new SceneLight(vec3(-20, 70, 20), vec3(0.5))));

	printf("Scene Defined Successfully\n");
}

struct Options {
	int width, height, maxDepth;
	float fov, aspectRatio, bias;
	vec3 backgroundColor;
};

Options options;

void setOptions() {

	options.width = 640;
	options.height = 480;
	options.fov = 90;
	options.backgroundColor = vec3(0);
	options.maxDepth = 5;
	options.bias = 0.00001f;

	printf("Options Set\n");
}

bool trace(vec3 &orig, vec3 &dir, float &tNear, int &index, vec2 &uv, SceneObject **hitObj) {

	for (int i = 0; i < sceneObjs.size(); i++) {
		
		float tNearI = INFINITY;
		int indexI;
		vec2 uvI;
		
		if (sceneObjs[i]->intersect(orig, dir, tNearI, indexI, uvI) && tNearI < tNear) {
			*hitObj = sceneObjs[i].get();
			tNear = tNearI;
			index = indexI;
			uv = uvI;
		}
	}
	
	bool hit = *hitObj != nullptr;

	//Prints if an object is hit
	//printf(hit ? "Hit = true\n" : "Hit = false\n");
	return (hit);
}

void fresnel(vec3 &dir, vec3 &norm, float &ior, float &kr) {
	//keep an eye on the int conversion. might be dodge
	float cosi = clamp(-1, 1, (int)dot(dir, norm));
	float etai = 1, etat = ior;
	if (cosi > 0) { swap(etai, etat); }
	
	//Compute sinT using Snell's law
	float sinT = etai / etat * sqrt(max(0.f, 1 - cosi * cosi));

	if (sinT >= 1) { kr = 1; } 
	else {
		float cosT = sqrt(max(0.f, 1 - sinT * sinT));
		cosi = abs(cosi);
		float Rs = ((etat * cosi) - (etai * cosT)) / ((etat * cosi) + (etai * cosT));
		float Rp = ((etai * cosi) - (etat * cosT)) / ((etai * cosi) + (etai * cosT));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
}

vec3 castRay(vec3 &orig, vec3 &dir, int depth, bool test = false) {
	
	if (depth > options.maxDepth) {
		return options.backgroundColor;
	}
	
	vec3 hitColor = options.backgroundColor;
	float tnear = INFINITY;
	vec2 uv(0);
	int index = 0;
	SceneObject *hitObj = nullptr;
	if (trace(orig, dir, tnear, index, uv, &hitObj)) {
		//printf("Index: %d\n", index);
		vec3 intersection = orig + dir * tnear;
		vec3 norm;
		vec2 st;
		hitObj->getSurfaceProperties(intersection, dir, index, uv, norm, st);
		vec3 tmp = intersection;
		switch (hitObj->materialType) {
			case REFLECTION_AND_REFRACTION:{
				vec3 reflectionDir = normalize(reflect(dir, norm));
				vec3 refractionDir = normalize(refract(dir, norm, hitObj->ior));

				vec3 reflectionRayOrig = (dot(reflectionDir, norm) < 0) ? intersection - norm * options.bias : intersection + norm * options.bias;
				vec3 refractionRayOrig = (dot(refractionDir, norm) < 0) ? intersection - norm * options.bias : intersection + norm * options.bias;

				vec3 reflectionColor = castRay(reflectionRayOrig, reflectionDir, depth + 1, 1);
				vec3 refractionColor = castRay(refractionRayOrig, refractionDir, depth + 1, 1);

				float kr;
				fresnel(dir, norm, hitObj->ior, kr);
				hitColor = reflectionColor * kr + refractionColor * (1 - kr);
				break;
			}
			case REFLECTION:{
				float kr;
				fresnel(dir, norm, hitObj->ior, kr);
				vec3 reflectionDir = reflect(dir, norm);
				vec3 reflectionRayOrig = (dot(reflectionDir, norm) < 0) ? intersection + norm * options.bias : intersection - norm * options.bias;
				hitColor = castRay(reflectionRayOrig, reflectionDir, depth + 1) * kr;
				break;
			}
			default: {
				vec3 lightAmt(0), specular(0);
				vec3 shadowPointOrig = (dot(dir, norm) < 0) ? intersection + norm * options.bias : intersection - norm * options.bias;

				for (int i = 0; i < sceneLights.size(); i++) {
					vec3 lightDir = sceneLights.at(i)->pos - intersection;
					float lightDist2 = dot(lightDir, lightDir);
					lightDir = normalize(lightDir);
					float LdotN = max(0.f, dot(lightDir, norm));
					SceneObject *shadowHitObj = nullptr;
					float tNearShadow = INFINITY;
					bool inShadow = trace(shadowPointOrig, lightDir, tNearShadow, index, uv, &shadowHitObj) && tNearShadow * tNearShadow < lightDist2;
					lightAmt += (1 - inShadow) * sceneLights.at(i)->intensity.x * LdotN;
					vec3 reflectionDir = reflect(-lightDir, norm);
					specular += powf(max(0.f, -dot(reflectionDir, dir)), hitObj->specular) * sceneLights.at(i)->intensity;
					hitColor = lightAmt * hitObj->evalDiffuseColor(st) * hitObj->kd + specular * hitObj->ks;
					break;
				}
			}
		}
	}

	//Prints color
	//printf("%d, %d, %d\n" , hitColor.x, hitColor.y, hitColor.z);
	return hitColor;
}

void render() {
	vector<vec3> frameBuffer;
	vec3 pixel;

	//double check this line
	float scale = tan(radians(options.fov * 0.5f));
	float imageAspectRatio = options.width / (float)options.height;
	vec3 orig(0);

	int index = 0;
	for (int j = 0; j < options.height; j++) {
		for (int i = 0; i < options.width; i++) {
			float x = (2 * (i + 0.5f) / options.width - 1) * imageAspectRatio * scale;
			float y = (1 - 2 * (j + 0.5f / options.height) * scale);
			vec3 direction = normalize(vec3(x, y, -1));
			pixel = castRay(orig, direction, 0);
			frameBuffer.push_back(pixel);
			index++;
		}
	}

	//Save framebuffer to file
	ofstream ofs;

	//Maybe change the file format?
	ofs.open("./out.ppm");

	ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
	for (int i = 0; i < options.height*options.width; i++) {
		char r = 255 * clamp(0, 1, (int)frameBuffer.at(i).x);
		char g = 255 * clamp(0, 1, (int)frameBuffer.at(i).y);
		char b = 255 * clamp(0, 1, (int)frameBuffer.at(i).z);
		ofs << r << g << b;
	}

	ofs.close();

	frameBuffer.clear();
}

//Runs on startup
int main(int argc, char **argv) {
	printf("Is Running\n");

	defineScene();

	setOptions();

	render();

	exit(0);
}

